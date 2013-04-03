#include <QDebug>

#include "cameraHandler.h"

GPPortInfoList* CameraHandler::portinfolist = NULL;
CameraAbilitiesList* CameraHandler::abilities = NULL;

void CameraHandler::init()
{
    qDebug() << "Initialize camera handler." << endl;
}

int CameraHandler::autodetect(CameraList *list, GPContext *context)
{
	int			ret, i;
	CameraList		*xlist = NULL;

	ret = gp_list_new (&xlist);
    qDebug() << "List new";
	if (ret < GP_OK) goto out;
	if (!portinfolist) {
		/* Load all the port drivers we have... */
		ret = gp_port_info_list_new (&portinfolist);
		if (ret < GP_OK) goto out;
		ret = gp_port_info_list_load (portinfolist);
		if (ret < 0) goto out;
		ret = gp_port_info_list_count (portinfolist);
		if (ret < 0) goto out;
	}
	/* Load all the camera drivers we have... */
	ret = gp_abilities_list_new (&abilities);
	if (ret < GP_OK) goto out;
	ret = gp_abilities_list_load (abilities, context);
	if (ret < GP_OK) goto out;

	/* ... and autodetect the currently attached cameras. */
        ret = gp_abilities_list_detect (abilities, portinfolist, xlist, context);
	if (ret < GP_OK) goto out;

	/* Filter out the "usb:" entry */
        ret = gp_list_count (xlist);
	if (ret < GP_OK) goto out;
	for (i=0;i<ret;i++) {
		const char *name, *value;

		gp_list_get_name (xlist, i, &name);
		gp_list_get_value (xlist, i, &value);
		if (!strcmp ("usb:",value)) continue;
		gp_list_append (list, name, value);
	}
out:
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
