#include <QDebug>
#include <QList>

#include "cameraHandler.h"

CameraHandler CameraHandler::m_instance=CameraHandler();

CameraHandler& CameraHandler::Instance()
{
	return m_instance;
}


int CameraHandler::handleError(int error, QString msg)
{
	qDebug() << msg;
    qDebug() << gp_result_as_string(error);
	if(error < GP_OK)
		throw "handler fail";
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
		cameras[i] = new QCamera(name, port, abilities, portinfolist);
	}
	return 0;
}

CameraHandler::CameraHandler()
{
    int ret;
    qDebug() << "Initialize camera handler." << endl;

	nCameras = 0;
	for(int i=0; i<MAX_CAMERA; i++)
		cameras[i] = NULL;
    context = gp_context_new();

    /* Port initialization */
    ret = gp_port_info_list_new(&portinfolist);
    if(ret < GP_OK) handleError(ret, "gp_port_info_list_new");
    ret = gp_port_info_list_load(portinfolist);
    if(ret < GP_OK) handleError(ret, "gp_port_info_list_load");
    ret = gp_port_info_list_count(portinfolist);
    if(ret < GP_OK) handleError(ret, "gp_port_info_list_count");

    /* Camera abilities initialization */
    ret = gp_abilities_list_new(&abilities);
    if(ret < GP_OK) handleError(ret, "gp_abilities_list_new");
    ret = gp_abilities_list_load(abilities, context);
    if(ret < GP_OK) handleError(ret, "gp_abilities_list_load");

	refreshCameraList();
}

CameraHandler::~CameraHandler()
{
	for(int i = 0; i < getNbCameras(); i++) {
		delete cameras[i];
		cameras[i] = NULL;
	}
}

int CameraHandler::autodetect(CameraList *list, GPContext *context)
{
	int			ret, i;
	CameraList		*xlist = NULL;
        
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
