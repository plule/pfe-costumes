#include "QCamera.h"

void idle_func(GPContext *context, void *data)
{
	(void)context;
	QCamera* camera = static_cast<QCamera*>(data);
	emit camera->idle(camera->getAbilities().model);
}

void error_func(GPContext *context, const char *format, va_list args, void *data)
{
	(void)args;
	(void)context;
	QCamera* camera = static_cast<QCamera*>(data);
	emit camera->error(format, camera->getAbilities().model);
}

void status_func(GPContext *context, const char *format, va_list args, void *data)
{
	(void)args;
	(void)context;
	QCamera* camera = static_cast<QCamera*>(data);
	emit camera->status(format, camera->getAbilities().model);
}

void message_func(GPContext *context, const char *format, va_list args, void *data)
{
	(void)args;
	(void)context;
	QCamera* camera = static_cast<QCamera*>(data);
	emit camera->message(format, camera->getAbilities().model);
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

unsigned int progress_start_func(GPContext *context, float target, const char *format, va_list args, void *data)
{
	(void)context;
	(void)args;
	int id = 0; // TODO : Assigner un identifiant unique
	QCamera* camera = static_cast<QCamera*>(data);
	emit camera->progress_start(id, format, target, camera->getAbilities().model);
	return id;
}

void progress_update_func(GPContext *context, unsigned int id, float current, void *data)
{
	(void)context;
	QCamera* camera = static_cast<QCamera*>(data);
	emit camera->progress_update(id, current, camera->getAbilities().model);
}

void progress_stop_func(GPContext *context, unsigned int id, void *data)
{
	(void)context;
	QCamera* camera = static_cast<QCamera*>(data);
	emit camera->progress_stop(id, camera->getAbilities().model);
}

int QCamera::handleError(int error, QString msg)
{
	qDebug() << msg;
    qDebug() << gp_result_as_string(error);
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
	gp_camera_init(camera, context);
	return 0;
}

QCamera::QCamera(const char *model, const char *port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist)
{
	context = gp_context_new();
	gp_context_set_idle_func (context, idle_func, this);
	gp_context_set_progress_funcs (context, progress_start_func, progress_update_func, progress_stop_func, this);
	gp_context_set_error_func(context, error_func, this);
	gp_context_set_status_func (context, status_func, this);
//	gp_context_set_question_func (context, question_func, this);
//	gp_context_set_cancel_func (context, cancel_func, this);
	gp_context_set_message_func (context, message_func, this);
	buildCamera(model, port, abilitiesList, portinfolist);
}

QString QCamera::getSummary()
{
	CameraText cameratext;
	gp_camera_get_summary(camera, &cameratext, context);
	return QString(cameratext.text);
}

int QCamera::captureToCamera(QString *cameraPath)
{
	CameraFilePath camera_file_path;
	int ret;
	// TODO : ensure memory is set to card
	ret = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);
	if(ret < GP_OK)
		return handleError(ret, "gp_camera_capture");
	cameraPath->clear();
	cameraPath->append(camera_file_path.folder);
	cameraPath->append(camera_file_path.name);
	return ret;
}

void QCamera::timeout()
{
	qDebug() << "timeout";
}

int QCamera::captureToFile(QFile *localFile)
{
	CameraFilePath camera_file_path;
	CameraFile *file;
	int ret;
	int fd;
	QTimer *timer = new QTimer(this);
	qDebug() << "timeoutset";
	connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
	timer->start(10);
	// TODO : ensure memory is set to RAM ?
	if(!localFile->open(QIODevice::WriteOnly))
		return -1;
	fd = localFile->handle();

	if((ret = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context)) < GP_OK)
		return handleError(ret, "capture");
	if((ret = gp_file_new_from_fd(&file, fd) < GP_OK))
	   return handleError(ret, "file new");
	if((ret = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name, GP_FILE_TYPE_NORMAL, file, context)) < GP_OK)
		return handleError(ret, "file get");
	if((ret = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name, context)) < GP_OK)
		return handleError(ret, "file rm");
	
	return 0;
}

int QCamera::captureToFile(QString path)
{
	return captureToFile(path.toLocal8Bit().data());
}

int QCamera::captureToFile(const char *name)
{
	QFile *localFile = new QFile(name);
	int ret=captureToFile(localFile);
	delete localFile;
	return ret;
}

