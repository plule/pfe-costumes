#include "cameraHandler.h"

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
    deinit();
    init();

    emit refreshed();
	return 0;
}

void CameraHandler::init()
{
    CameraList *list;
    const char *name, *port;
    int ret;
    context = gp_context_new();

    /* Port initialization */
    if ((ret = gp_port_info_list_new(&portinfolist)) < GP_OK) handleError(ret, "gp_port_info_list_new");
    if ((ret = gp_port_info_list_load(portinfolist)) < GP_OK) handleError(ret, "gp_port_info_list_load");
    if ((ret = gp_port_info_list_count(portinfolist)) < GP_OK) handleError(ret, "gp_port_info_list_count");

    /* Camera abilities initialization */
    if ((ret = gp_abilities_list_new(&abilities)) < GP_OK) handleError(ret, "gp_abilities_list_new");
    if ((ret = gp_abilities_list_load(abilities, context)) < GP_OK) handleError(ret, "gp_abilities_list_load");

    /* Camera listing */
    if ((ret = gp_list_new(&list)) < GP_OK) handleError(ret, "gp_list_new");


    nCameras = autodetect(list, context);

    if(nCameras > MAX_CAMERA)
        nCameras = MAX_CAMERA;
    for(int i = 0; i < nCameras; i++) {
        /* Get camera name and port name*/
        gp_list_get_name(list, i, &name);
        gp_list_get_value(list, i, &port);

        /* Create the camera object from the name and the port */
        cameras[i] = new QCamera(name, port, abilities, portinfolist);
        cameras[i]->moveToThread(&cameraThread);
    }
    emit message(QString::number(nCameras) + " camera(s) found.");
}

void CameraHandler::deinit()
{
    int ret;

    for(int i = 0; i < MAX_CAMERA; i++) {
        if(cameras[i] != NULL) {
            delete cameras[i];
            cameras[i] = NULL;
        }
    }

    if((ret = gp_abilities_list_free(abilities)) < GP_OK) handleError(ret, "gp_abilities_list_free");
    if((ret = gp_port_info_list_free(portinfolist)) < GP_OK) handleError(ret, "gp_port_info_list_free");

    gp_context_unref(context);
}

CameraHandler::CameraHandler()
{
    qDebug() << "Initialize camera handler." << endl;

	nCameras = 0;
	for(int i=0; i<MAX_CAMERA; i++)
		cameras[i] = NULL;
    cameraThread.start();
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
