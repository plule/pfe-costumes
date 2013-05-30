#include "camerahandler.h"

namespace QPhoto
{

int CameraHandler::handleError(int error, QString msg)
{
    qDebug() << msg;
    qDebug() << gp_result_as_string(error);
    return error;
}

int CameraHandler::getNbCameras()
{
    return m_nCameras;
}

int CameraHandler::getCameras(QCamera ***cameras)
{
    (*cameras) = m_cameras;
    return m_nCameras;
}

int CameraHandler::refreshCameraList()
{
    deinit();
    init();

    emit refreshed();
	return 0;
}

int CameraHandler::init()
{
    CameraList *list;
    const char *name, *port;
    int ret;
    m_context = gp_context_new();

    /* Port initialization */
    if ((ret = gp_port_info_list_new(&m_portinfolist)) < GP_OK) return handleError(ret, "gp_port_info_list_new");
    if ((ret = gp_port_info_list_load(m_portinfolist)) < GP_OK) return handleError(ret, "gp_port_info_list_load");
    if ((ret = gp_port_info_list_count(m_portinfolist)) < GP_OK) return handleError(ret, "gp_port_info_list_count");

    /* Camera abilities initialization */
    if ((ret = gp_abilities_list_new(&m_abilities)) < GP_OK) return handleError(ret, "gp_abilities_list_new");
    if ((ret = gp_abilities_list_load(m_abilities, m_context)) < GP_OK) return handleError(ret, "gp_abilities_list_load");

    /* Camera listing */
    if ((ret = gp_list_new(&list)) < GP_OK) return handleError(ret, "gp_list_new");


    m_nCameras = autodetect(list, m_context);

    if(m_nCameras > MAX_CAMERA)
        m_nCameras = MAX_CAMERA;
    for(int i = 0; i < m_nCameras; i++) {
        /* Get camera name and port name*/
        gp_list_get_name(list, i, &name);
        gp_list_get_value(list, i, &port);

        /* Create the camera object from the name and the port */
        m_cameras[i] = new QCamera(name, port, m_abilities, m_portinfolist);
    }
    emit message(tr("%n camera(s) found.", "", m_nCameras));
    return GP_OK;
}

int CameraHandler::deinit()
{
    int ret;

    for(int i = 0; i < MAX_CAMERA; i++) {
        if(m_cameras[i] != NULL) {
            delete m_cameras[i];
            m_cameras[i] = NULL;
        }
    }

    if((ret = gp_abilities_list_free(m_abilities)) < GP_OK) return handleError(ret, "gp_abilities_list_free");
    if((ret = gp_port_info_list_free(m_portinfolist)) < GP_OK) return handleError(ret, "gp_port_info_list_free");

    gp_context_unref(m_context);

    return GP_OK;
}

CameraHandler::CameraHandler()
{
    m_nCameras = 0;
	for(int i=0; i<MAX_CAMERA; i++)
        m_cameras[i] = NULL;
}

CameraHandler::~CameraHandler()
{
	for(int i = 0; i < getNbCameras(); i++) {
        delete m_cameras[i];
        m_cameras[i] = NULL;
	}
}

int CameraHandler::autodetect(CameraList *list, GPContext *context)
{
	int			ret, i;
	CameraList		*xlist = NULL;
        
	ret = gp_list_new (&xlist);
    if(ret < GP_OK) return handleError(ret, "gp_list_new");
	/* ... and autodetect the currently attached cameras. */
    ret = gp_abilities_list_detect (m_abilities, m_portinfolist, xlist, context);
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

}
