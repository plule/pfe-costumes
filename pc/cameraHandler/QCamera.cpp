#include "QCamera.h"

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
	(void)context;
    QString error_msg;
    error_msg.vsprintf(format, args);
#else
void error_func(GPContext *context, const char *error_msg, void *data)
{
#endif
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
	(void)context;
    QString status;
    status.vsprintf(format, args);
#else
void status_func(GPContext *context, const char *status, void *data)
{
#endif
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
	(void)context;
    QString message;
    message.vsprintf(format, args);
#else
void message_func(GPContext *context, const char *message, void *data)
{
#endif
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
	(void)context;
    QString task;
    task.vsprintf(format, args);
#else
unsigned int progress_start_func(GPContext *context, float target, const char *task, void *data)
{
#endif
	int id = 0; // TODO : Assigner un identifiant unique
	QCamera* camera = static_cast<QCamera*>(data);
    emit camera->progress_start(task, target);
	return id;
}

void progress_update_func(GPContext *context, unsigned int id, float current, void *data)
{
	(void)context;
	QCamera* camera = static_cast<QCamera*>(data);
    emit camera->progress_update(int(current));
}

void progress_stop_func(GPContext *context, unsigned int id, void *data)
{
	(void)context;
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
	return abilities;
}

QCamera::QCamera()
{
	camera = NULL;
	context = NULL;
}

QCamera::~QCamera()
{
	if(camera)
		gp_camera_exit(camera, context);
}

int QCamera::buildCamera(const char *model, const char *port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist)
{
    int ret, model_index, port_index;
	/* Create the camera object */
    if((ret = gp_camera_new(&camera)) < GP_OK)
		return handleError(ret, "gp_camera_new");

	/* Assign the abilities */
	if ((model_index = gp_abilities_list_lookup_model(abilitiesList, model)) < GP_OK)
		return handleError(model_index, "gp_abilities_list_lookup_model");
	if ((ret = gp_abilities_list_get_abilities(abilitiesList, model_index, &abilities)) < GP_OK)
		return handleError(ret, "gp_abilities_list_get_abilities");
	if ((ret = gp_camera_set_abilities(camera, abilities)) < GP_OK)
		return handleError(ret, "gp_camera_set_abilities");

	/* Assign to a port */
	if ((port_index = gp_port_info_list_lookup_path(portinfolist, port)) < GP_OK)
		return handleError(ret, "gp_port_info_list_lookup_path");
	if ((ret = gp_port_info_list_get_info(portinfolist, port_index, &portinfo)) < GP_OK)
		return handleError(ret, "gp_port_info_list_get_info");
	if ((ret = gp_camera_set_port_info(camera, portinfo)) < GP_OK)
		return handleError(ret, "gp_camera_set_port_info");

	/* Init the connection */
    if ((ret = gp_camera_init(camera, context)) < GP_OK)
        return handleError(ret, "gp_camera_init");
    return GP_OK;
}

QCamera::QCamera(const char *model, const char *port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist)
{
    int ret;
	context = gp_context_new();
	gp_context_set_idle_func (context, idle_func, this);
	gp_context_set_progress_funcs (context, progress_start_func, progress_update_func, progress_stop_func, this);
	gp_context_set_error_func(context, error_func, this);
	gp_context_set_status_func (context, status_func, this);
//	gp_context_set_question_func (context, question_func, this);
//	gp_context_set_cancel_func (context, cancel_func, this);
	gp_context_set_message_func (context, message_func, this);

    if((ret = buildCamera(model, port, abilitiesList, portinfolist)) != GP_OK)
        throw CameraException(QString("Failed to connect to camera : ") + QString(gp_result_as_string(ret)));

    this->setObjectName(QString(model));
}

QString QCamera::getSummary()
{
	CameraText cameratext;
	gp_camera_get_summary(camera, &cameratext, context);
	return QString(cameratext.text);
}

void QCamera::captureToCamera(QString *cameraPath)
{
	CameraFilePath camera_file_path;
	int ret;
	// TODO : ensure memory is set to card
	ret = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);
	if(ret < GP_OK)
    {
        handleError(ret, "gp_camera_capture");
        return;
    }
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

	// TODO : ensure memory is set to RAM ?

    if((ret = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context)) < GP_OK)
        return handleError(ret, "capture");

    if(!localFile->open(QIODevice::WriteOnly))
        return -1; // TODO : own error
    fd = localFile->handle();

    if((ret = gp_file_new_from_fd(&file, fd) < GP_OK)) {
        localFile->close();
        return handleError(ret, "file new");
    }
    if((ret = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name, GP_FILE_TYPE_NORMAL, file, context)) < GP_OK) {
        localFile->close();
        return handleError(ret, "file get");
    }
    localFile->close();
    if((ret = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name, context)) < GP_OK)
        return handleError(ret, "file rm");
    return GP_OK;
}

void QCamera::captureToFile(QString path, int nbTry)
{
    QFile localFile(path);
    for(int i = 0; i < nbTry; i++) {
        if (captureToFile(&localFile) == GP_OK) {
            emit captured(path);
            return;
        }
        Sleeper().sleep(1);
    }
    emit operation_failed(QString("Could not capture image after ") + QString::number(nbTry) + " times.");
}

void QCamera::captureToFile(const char *name, int nbTry)
{
    captureToFile(QString(name), nbTry);
}
}
