#include "QCamera.h"

int QCamera::handleError(int error, QString msg)
{
	qDebug() << msg;
    qDebug() << gp_result_as_string(error);
	throw "Failed to init cam";
    return error;
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

QCamera::QCamera(const char *model, const char *port, GPContext *a_context, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist)
{
	context = a_context;
	buildCamera(model, port, abilitiesList, portinfolist);
}

QString QCamera::getSummary()
{
	CameraText cameratext;
	gp_camera_get_summary(camera, &cameratext, context);
	return QString(cameratext.text);
}

int QCamera::capture()
{
	CameraFilePath camera_file_path;
	int ret;
	strcpy(camera_file_path.folder, "/");
	strcpy(camera_file_path.name, "test.jpg");
	qDebug() << context;
	ret = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);
	qDebug() << gp_result_as_string(ret);
	return ret;
}
