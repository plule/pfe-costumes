#include <QDebug>
#include <QList>

#include "cameraHandler.h"

GPPortInfoList* CameraHandler::portinfolist = NULL;
CameraAbilitiesList* CameraHandler::abilities = NULL;
bool CameraHandler::initialized = false;
QCamera* CameraHandler::cameras[MAX_CAMERA] = {NULL};

int CameraHandler::handleError(int error, QString msg)
{
	qDebug() << msg;
    qDebug() << gp_result_as_string(error);
    return error;
}

int CameraHandler::refreshCameraList()
{
    CameraList *list;
    GPContext *context;
    const char *name, *port;
    int ret;
    int nbcam;

	for(int i = 0; i < MAX_CAMERA; i++) {
		if(cameras[i] != NULL) {
			delete cameras[i];
			cameras[i] = NULL;
		}
	}

    if(!initialized){
        init();
    } else {
        context = gp_context_new();
        ret = gp_list_new(&list);
        if(ret < GP_OK) return handleError(ret, "gp_list_new");
        nbcam = autodetect(list, context);
		if(nbcam > MAX_CAMERA)
			nbcam = MAX_CAMERA;
        for(int i = 0; i < nbcam; i++) {
			/* Get camera name and port name*/
            gp_list_get_name(list, i, &name);
            gp_list_get_value(list, i, &port);
			
			/* Create the camera object from the name and the port */
			cameras[i] = new QCamera(name, port, abilities, portinfolist);
		}
	}
	return 0;
}

int CameraHandler::init()
{
    int ret;
    GPContext *context;
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

    initialized = true;
    refreshCameraList();
    return 0;
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

int CameraHandler::count_cameras()
{
    GPContext *context;
    CameraList *list;
    int ret;

    context = gp_context_new();
    ret = gp_list_new(&list);
    if(ret < GP_OK) return ret;

    return autodetect(list, context);
}
