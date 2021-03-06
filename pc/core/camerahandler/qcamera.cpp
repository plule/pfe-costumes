#include "qcamera.h"

namespace QPhoto
{

void idle_func(GPContext *context, void *data)
{
	(void)context;
	QCamera* camera = static_cast<QCamera*>(data);
    emit camera->idle();
}

#ifdef LEGACY_GPHOTO
void
#ifdef __GNUC__
__attribute__((__format__(printf,2,0)))
#endif
error_func(GPContext *context, const char *format, va_list args, void *data)
{
    QString error_msg;
    error_msg.vsprintf(format, args);
#else
void error_func(GPContext *context, const char *error_msg_char, void *data)
{
    QString error_msg(error_msg_char);
#endif
    (void)context;
    QCamera* camera = static_cast<QCamera*>(data);
    camera->appendError(QString::fromUtf8(error_msg.toLatin1()));
    emit camera->error(error_msg);
}

#ifdef LEGACY_GPHOTO
void
#ifdef __GNUC__
__attribute__((__format__(printf,2,0)))
#endif
status_func(GPContext *context, const char *format, va_list args, void *data)
{
    QString status;
    status.vsprintf(format, args);
#else
void status_func(GPContext *context, const char *status_char, void *data)
{
    QString status(status_char);
#endif
    (void)context;
    QCamera* camera = static_cast<QCamera*>(data);
    emit camera->status(status.toLatin1());
}

#ifdef LEGACY_GPHOTO
void
#ifdef __GNUC__
__attribute__((__format__(printf,2,0)))
#endif
message_func(GPContext *context, const char *format, va_list args, void *data)
{
    QString message;
    message.vsprintf(format, args);
#else
void message_func(GPContext *context, const char *message_char, void *data)
{
    QString message(message_char);
#endif
    (void)context;
	QCamera* camera = static_cast<QCamera*>(data);
    emit camera->message(message);
}

/*
  TODO : deal with answer
*/
/*static GPContextFeedback question_func(GPContext *context, const char *format, va_list args, void *data)
{

}*/

/*static GPContextFeedback cancel_func(GPContext *context, void *data)
{

}*/

#ifdef LEGACY_GPHOTO
unsigned int
#ifdef __GNUC__
__attribute__((__format__(printf,3,0)))
#endif
progress_start_func(GPContext *context, float target, const char *format, va_list args, void *data)
{
    QString task;
    task.vsprintf(format, args);
#else
unsigned int progress_start_func(GPContext *context, float target, const char *task_char, void *data)
{
    QString task(task_char);
#endif
    (void)context;
	QCamera* camera = static_cast<QCamera*>(data);
    emit camera->progress_start(task, target);
    return 0;
}

void progress_update_func(GPContext *context, unsigned int id, float current, void *data)
{
	(void)context;
    (void)id; // no multiple camera at a time, useless
	QCamera* camera = static_cast<QCamera*>(data);
    emit camera->progress_update(int(current));
}

void progress_stop_func(GPContext *context, unsigned int id, void *data)
{
	(void)context;
    (void)id;
	QCamera* camera = static_cast<QCamera*>(data);
    emit camera->progress_stop();
}

int QCamera::handleError(int error, QString msg)
{
    qDebug() << QString(gp_result_as_string(error)) + " : " + msg;
    return error;
}

CameraAbilities QCamera::getAbilities()
{
    return m_abilities;
}

QCamera::QCamera()
{
    m_camera = NULL;
    m_context = NULL;
    m_connected = false;
    m_busy = false;
}

QCamera::~QCamera()
{
    qDebug() << "~QCamera";
    if(m_camera)
        gp_camera_exit(m_camera, m_context);
}

int QCamera::buildCamera(const char *model, const char *port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist)
{
    int ret, model_index, port_index;
    GPPortInfo portinfo;
	/* Create the camera object */
    R_GP_CALL(ret, gp_camera_new, &m_camera);

	/* Assign the abilities */
    R_GP_CALL(model_index, gp_abilities_list_lookup_model, abilitiesList, model);
    R_GP_CALL(ret, gp_abilities_list_get_abilities, abilitiesList, model_index, &m_abilities);
    R_GP_CALL(ret, gp_camera_set_abilities, m_camera, m_abilities);

	/* Assign to a port */
    R_GP_CALL(port_index, gp_port_info_list_lookup_path, portinfolist, port);
    R_GP_CALL(ret, gp_port_info_list_get_info, portinfolist, port_index, &portinfo);
    R_GP_CALL(ret, gp_camera_set_port_info, m_camera, portinfo);

	/* Init the connection */
    R_GP_CALL(ret, gp_camera_init, m_camera, m_context);
    return GP_OK;
}

QCamera::QCamera(const char *model, const char *port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist)
{
    int ret;
    this->m_model = QString(model);
    this->m_port = QString(port);

    m_context = gp_context_new();
    gp_context_set_idle_func (m_context, idle_func, this);
    gp_context_set_progress_funcs (m_context, progress_start_func, progress_update_func, progress_stop_func, this);
    gp_context_set_error_func(m_context, error_func, this);
    gp_context_set_status_func (m_context, status_func, this);
//	gp_context_set_question_func (context, question_func, this);
//	gp_context_set_cancel_func (context, cancel_func, this);
    gp_context_set_message_func (m_context, message_func, this);

    if((ret = buildCamera(model, port, abilitiesList, portinfolist)) != GP_OK)
        this->m_connected = false;
    else
        this->m_connected = true;
    this->setObjectName(QString(model));
}

QString QCamera::getSummary()
{
	CameraText cameratext;
    gp_camera_get_summary(m_camera, &cameratext, m_context);
	return QString(cameratext.text);
}

int QCamera::getFile(QString folder, QString name, QFile *localFile)
{
    CameraFile *file;
    int fd,ret;

    if(!localFile->open(QIODevice::WriteOnly))
        return -1;

    fd = localFile->handle();
    ret = gp_file_new_from_fd(&file, fd);
    if (ret < GP_OK) {
        localFile->close();
        return handleError(ret, "file new");
    }

    ret = gp_camera_file_get(m_camera, folder.toStdString().c_str(), name.toStdString().c_str(), GP_FILE_TYPE_NORMAL, file, m_context);
    if(ret < GP_OK) {
        localFile->close();
        gp_file_unref(file);
        return handleError(ret, "file get");
    }

    localFile->close();
    gp_file_unref(file);
    CameraFileInfo info;
    gp_camera_file_get_info(m_camera, folder.toStdString().c_str(), name.toStdString().c_str(), &info, m_context);
    if(!(info.file.fields & GP_FILE_INFO_STATUS && info.file.status == GP_FILE_STATUS_DOWNLOADED ))
        gp_camera_file_delete(m_camera, folder.toStdString().c_str(), name.toStdString().c_str(), m_context);

    return GP_OK;
}

int QCamera::deleteFile(QString folder, QString name)
{
    qDebug() << "deleting " << folder << name;
    return handleError(gp_camera_file_delete(m_camera, folder.toStdString().c_str(), name.toStdString().c_str(), m_context), "delete");
}

int QCamera::captureToQFile(QFile *localFile, int nbTry)
{
    QPair<QString,QString> path = captureToCamera(nbTry);
    int ret;
    ret = getFile(path.first, path.second, localFile);
    if(ret == GP_OK)
        deleteFile(path.first, path.second);
    return ret;
}

int QCamera::captureToFile(QString path, int nbTry)
{
    QFile localFile(path);
    int ret = captureToQFile(&localFile, nbTry);
    emit downloaded(ret, m_errors);
    return ret;
}

QPair<QString, QString> QCamera::captureToCamera(int nbTry)
{
    m_busy = true;
    m_errors.clear();
    if(isConnected()) {
        int ret;
        for(int i=0; i<nbTry; i++) {
            qDebug() << "trigger";
            ret = gp_camera_trigger_capture(m_camera, m_context);;
            if(ret != GP_OK) {
                m_busy = false;
                emit finished(Error, m_errors);
                return QPair<QString, QString>();
            }

            CameraEventType event;
            void *data;
            do {
                qDebug() << "wait for event";
                ret = gp_camera_wait_for_event(m_camera, 3000, &event, &data, m_context);
                qDebug() << event;
                if(ret != GP_OK || event == GP_EVENT_TIMEOUT)
                {
                    m_busy = false;
                    emit finished(Error, m_errors);
                    return QPair<QString, QString>();
                }
            } while(event != GP_EVENT_FILE_ADDED);

            CameraFilePath *camera_file_path = (CameraFilePath*)data;
            qDebug() << camera_file_path->folder << camera_file_path->name;
            emit finished(OK, m_errors);
            return QPair<QString, QString>(camera_file_path->folder, camera_file_path->name);
            /*if(ret == GP_OK) {
                m_busy = false;
                return QPair<QString,QString>();//(QString(camera_file_path.folder), QString(camera_file_path.name));
            }*/
            this->thread()->sleep(0.5);
        }
        m_busy = false;
        return QPair<QString,QString>();
    } else {
        m_busy = false;
        emit finished(NotConnected, m_errors);
        return QPair<QString, QString>(); // generic error
    }
}

void QCamera::appendError(QString error)
{
    m_errors.append(error);
}

QString QCamera::getPort() const
{
    return m_port;
}

bool QCamera::isConnected() const
{
    return m_connected;
}

QString QCamera::getModel() const
{
    return m_model;
}

bool QCamera::busy() const
{
    return m_busy;
}
}
