#ifndef __WP_MIDDLEWARE_H
#define __WP_MIDDLEWARE_H

#include "middleware_utils.h"
#include "sample_utils.h"
#include "vi_vo_utils.h"

#include <core/utils/vpss_helper.h>
#include <cvi_comm.h>
#include <rtsp.h>
#include <sample_comm.h>
#include "cvi_tdl.h"

int middleware_init(SAMPLE_TDL_MW_CONTEXT *stMWContext);

#endif
