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
void error_func(GPContext *context, const char *error_msg, void *data)
{
#endif
    (void)context;
    QCamera* camera = static_cast<QCamera*>(data);
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
void status_func(GPContext *context, const char *status, void *data)
{
#endif
    (void)context;
    QCamera* camera = static_cast<QCamera*>(data);
    emit camera->status(status);
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
void message_func(GPContext *context, const char *message, void *data)
{
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
unsigned int progress_start_func(GPContext *context, float target, const char *task, void *data)
{
#endif
    (void)context;
	int id = 0; // TODO : Assigner un identifiant unique
	QCamera* camera = static_cast<QCamera*>(data);
    emit camera->progress_start(task, target);
	return id;
}

void progress_update_func(GPContext *context, unsigned int id, float current, void *data)
{
	(void)context;
    (void)id; // TODO utiliser l'id
	QCamera* camera = static_cast<QCamera*>(data);
    emit camera->progress_update(int(current));
}

void progress_stop_func(GPContext *context, unsigned int id, void *data)
{
	(void)context;
    (void)id; // TODO utiliser l'id
	QCamera* camera = static_cast<QCamera*>(data);
    emit camera->progress_stop();
}

int QCamera::handleError(int error, QString msg)
{
    qDebug() << "error";
    qDebug() << QString(gp_result_as_string(error)) + " : " + msg;
    return error;
}

CameraAbilities QCamera::getAbilities()
{
    return m_abilities;
}

QTimer *QCamera::getWatchdog()
{
    return m_watchdog;
}

QCamera::QCamera()
{
    m_camera = NULL;
    m_context = NULL;
    // TODO : init thread and watchdog ?
}

QCamera::~QCamera()
{
    if(m_camera)
        gp_camera_exit(m_camera, m_context);
    delete m_watchdog;
    m_camThread.exit();
    m_camThread.wait(); // TODO : dangerous ??
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
        throw CameraException(QString("Failed to connect to camera : ") + QString(gp_result_as_string(ret)));
    m_watchdog = new QTimer(QApplication::instance()->thread());
    m_watchdog->setSingleShot(true);
    m_watchdog->setInterval(5000);
    connect(this, SIGNAL(camera_answered()), m_watchdog, SLOT(stop()));
    connect(this, SIGNAL(wait_for_camera_answer()), m_watchdog, SLOT(start()));
    m_camThread.start();
    this->moveToThread(&m_camThread);
    this->setObjectName(QString(model));
}

QString QCamera::getSummary()
{
	CameraText cameratext;
    gp_camera_get_summary(m_camera, &cameratext, m_context);
	return QString(cameratext.text);
}

void QCamera::captureToCamera(QString *cameraPath)
{
	CameraFilePath camera_file_path;
	int ret;
    // TODO : ensure memory is set to card
    GP_CALL(ret, gp_camera_capture, m_camera, GP_CAPTURE_IMAGE, &camera_file_path, m_context);
    if(ret < GP_OK) return;
	cameraPath->clear();
	cameraPath->append(camera_file_path.folder);
	cameraPath->append(camera_file_path.name);
}

int QCamera::captureToFile(QFile *localFile)
{
	CameraFilePath camera_file_path;
	CameraFile *file;
	int ret;
	int fd;
    qDebug() << "capturing";
    R_GP_CALL(ret, gp_camera_capture, m_camera, GP_CAPTURE_IMAGE, &camera_file_path, m_context);
    qDebug() << "downloading";
    emit downloading();

    if(!localFile->open(QIODevice::WriteOnly))
        return -1; // TODO : own error
    fd = localFile->handle();

    GP_CALL(ret, gp_file_new_from_fd, &file, fd);
    if (ret < GP_OK) {
        localFile->close();
        return handleError(ret, "file new");
    }

    GP_CALL(ret, gp_camera_file_get, m_camera, camera_file_path.folder, camera_file_path.name, GP_FILE_TYPE_NORMAL, file, m_context);
    if(ret < GP_OK) {
        localFile->close();
        return handleError(ret, "file get");
    }

    localFile->close();

    R_GP_CALL(ret, gp_camera_file_delete, m_camera, camera_file_path.folder, camera_file_path.name, m_context);
    return GP_OK;
}

void QCamera::_captureToFile(QString path, int nbTry)
{
    QFile localFile(path);
    for(int i = 0; i < nbTry; i++) {
        int ret = captureToFile(&localFile);
        if( ret == GP_OK) {
            emit captured(path);
            emit operation_success();
            return;
        }
        Sleeper().sleep(1);
    }
    emit operation_failed(tr("Could not capture image after %n try(es)", "", nbTry));
}

void QCamera::captureToFile(const char *name, int nbTry)
{
    captureToFile(QString(name), nbTry);
}

void QCamera::captureToFile(QString path, int nbTry)
{
    QMetaObject::invokeMethod(this, "_captureToFile", Qt::QueuedConnection, Q_ARG(QString, path), Q_ARG(int, nbTry));

}
QString QCamera::getPort() const
{
    return m_port;
}

QString QCamera::getModel() const
{
    return m_model;
}
}
