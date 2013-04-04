#include "QCamera.h"


int QCamera::handleError(int error, QString msg)
{
	qDebug() << msg;
    qDebug(gp_result_as_string(error));
	throw "Failed to init cam";
    return error;
}


QCamera::QCamera()
{
}

int QCamera::buildCamera(const char *model, const char *port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist)
{
    CameraAbilities abilities;
    GPPortInfo portinfo;
    Camera *camera;
    int ret, model_index, port_index;

    if((ret = gp_camera_new(&camera)) < GP_OK)
		return handleError(ret, "gp_camera_new");
	if ((model_index = gp_abilities_list_lookup_model(abilitiesList, model)) < GP_OK)
		return handleError(model_index, "gp_abilities_list_lookup_model");
	if ((ret = gp_abilities_list_get_abilities(abilitiesList, model_index, &abilities)) < GP_OK)
		return handleError(ret, "gp_abilities_list_get_abilities");
	if ((ret = gp_camera_set_abilities(camera, abilities)) < GP_OK)
		return handleError(ret, "gp_camera_set_abilities");
	if ((port_index = gp_port_info_list_lookup_path(portinfolist, port)) < GP_OK)
		return handleError(ret, "gp_port_info_list_lookup_path");
	if ((ret = gp_port_info_list_get_info(portinfolist, port_index, &portinfo)) < GP_OK)
		return handleError(ret, "gp_port_info_list_get_info");
	if ((ret = gp_camera_set_port_info(camera, portinfo)) < GP_OK)
		return handleError(ret, "gp_camera_set_port_info");
	return 0;
}

QCamera::QCamera(const char *model, const char *port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist)
{
	buildCamera(model, port, abilitiesList, portinfolist);
}

int QCamera::capture()
{
    // TODO
    //gp_camera_capture
    return 0;
}
