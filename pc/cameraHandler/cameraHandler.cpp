#include <QDebug>
#include <QList>

#include "cameraHandler.h"

GPPortInfoList* CameraHandler::portinfolist = NULL;
GPContext* CameraHandler::context = NULL;
CameraAbilitiesList* CameraHandler::abilities = NULL;
bool CameraHandler::initialized = false;
QCamera* CameraHandler::cameras[MAX_CAMERA] = {NULL};
int CameraHandler::nCameras = 0;

static void idle_func(GPContext *context, void *data)
{
//	qDebug()<<"ctxt idle";
}

static void error_func(GPContext *context, const char *format, va_list args, void *data)
{
	qDebug() << "ctxt error : " << format;
}

static void status_func(GPContext *context, const char *format, va_list args, void *data)
{
	qDebug() << "ctxt status : " << format;
}

static void message_func(GPContext *context, const char *format, va_list args, void *data)
{
	qDebug() << "ctxt message : " << format;
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

static unsigned int progress_start_func(GPContext *context, float target, const char *format, va_list args, void *data)
{
	qDebug() << "ctxt start : target(" << target << ") format (" << format << ")";
	return 0;// TODO : ?
}

static void progress_update_func(GPContext *context, unsigned int id, float current, void *data)
{
	qDebug() << "ctxt progress : id(" << id << ") current (" << current << ")";
}

static void progress_stop_func(GPContext *context, unsigned int id, void *data)
{
	qDebug() << "ctxt stop : " << id;
}


int CameraHandler::handleError(int error, QString msg)
{
	qDebug() << msg;
    qDebug() << gp_result_as_string(error);
    return error;
}

int CameraHandler::getNbCameras()
{
	return nCameras;
}

int CameraHandler::getCameras(QCamera ***cameras)
{
	(*cameras) = CameraHandler::cameras;
	return nCameras;
}

int CameraHandler::refreshCameraList()
{
    CameraList *list;
    const char *name, *port;
    int ret;

	for(int i = 0; i < MAX_CAMERA; i++) {
		if(cameras[i] != NULL) {
			delete cameras[i];
			cameras[i] = NULL;
		}
	}

    if(!initialized){
        init();
    } else {
        ret = gp_list_new(&list);
        if(ret < GP_OK) return handleError(ret, "gp_list_new");
        nCameras = autodetect(list, context);
		if(nCameras > MAX_CAMERA)
			nCameras = MAX_CAMERA;
        for(int i = 0; i < nCameras; i++) {
			/* Get camera name and port name*/
            gp_list_get_name(list, i, &name);
            gp_list_get_value(list, i, &port);

			/* Create the camera object from the name and the port */
			cameras[i] = new QCamera(name, port, context, abilities, portinfolist);
		}
	}
	return 0;
}

int CameraHandler::init()
{
    int ret;
    qDebug() << "Initialize camera handler." << endl;

    context = gp_context_new();

    /* Port initialization */
    ret = gp_port_info_list_new(&portinfolist);
    if(ret < GP_OK) return handleError(ret, "gp_port_info_list_new");
    ret = gp_port_info_list_load(portinfolist);
    if(ret < GP_OK) return handleError(ret, "gp_port_info_list_load");
    ret = gp_port_info_list_count(portinfolist);
    if(ret < GP_OK) return handleError(ret, "gp_port_info_list_count");

    /* Camera abilities initialization */
    ret = gp_abilities_list_new(&abilities);
    if(ret < GP_OK) return handleError(ret, "gp_abilities_list_new");
    ret = gp_abilities_list_load(abilities, context);
    if(ret < GP_OK) return handleError(ret, "gp_abilities_list_load");

	gp_context_set_idle_func (context, idle_func, NULL);
	gp_context_set_progress_funcs (context, progress_start_func, progress_update_func, progress_stop_func, NULL);
	gp_context_set_error_func(context, error_func, NULL);
	gp_context_set_status_func (context, status_func, NULL);
//	gp_context_set_question_func (context, question_func, NULL);
//	gp_context_set_cancel_func (context, cancel_func, NULL);
	gp_context_set_message_func (context, message_func, NULL);
    initialized = true;
	refreshCameraList();
	gp_context_message(context, "hello hello");
	return 0;
}

int CameraHandler::close()
{
	for(int i = 0; i < getNbCameras(); i++) {
		delete cameras[i];
		cameras[i] = NULL;
	}
	return 0; // TODO detect error
}

int CameraHandler::autodetect(CameraList *list, GPContext *context)
{
	int			ret, i;
	CameraList		*xlist = NULL;
    if(!initialized) {
        ret = init();
        if(ret < GP_OK)
            return ret;
    }
        
	ret = gp_list_new (&xlist);
    if(ret < GP_OK) return handleError(ret, "gp_list_new");
	/* ... and autodetect the currently attached cameras. */
    ret = gp_abilities_list_detect (abilities, portinfolist, xlist, context);
	if (ret < GP_OK) return handleError(ret, "gp_abilities_list_detect");

	/* Filter out the "usb:" entry */
    ret = gp_list_count (xlist);
	if (ret < GP_OK) return handleError(ret, "gp_list_count");
	for (i=0;i<ret;i++) {
		const char *name, *value;

		gp_list_get_name (xlist, i, &name);
		gp_list_get_value (xlist, i, &value);
		if (!strcmp ("usb:",value)) continue;
		gp_list_append (list, name, value);
	}

	gp_list_free (xlist);
	return gp_list_count(list);
}
