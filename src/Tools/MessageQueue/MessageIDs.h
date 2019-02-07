/**
* @file MessageIDs.h
*
* Declaration of ids for debug messages.
*
* @author Martin LÃ¶tzsch
*/

#pragma once

#include "Tools/Streams/Enum.h"

/**
* IDs for debug messages
*
* To distinguish debug messages, they all have an id.
*/


ENUM(MessageID,
{,
 undefined,
 idProcessBegin,
 idProcessFinished,

 idImage,
 idJPEGImage,
 idFrameInfo,
 idGroundTruthWorldState,
 idCameraMatrix,
 idCameraInfo,
 idImageCoordinateSystem,
 idGroundTruthRobotPose,
 idGroundTruthBallModel,
 idGroundTruthOdometryData,
 idGroundTruthOrientationData,
 idCustomImage,
 idLogReUseMe2, // to be removed
 idLogReUseMe3, // to be removed
 idLogReUseMe4, // to be removed
 idLogReUseMe5, // to be removed
 idLogReUseMe6, // to be removed
 numOfDataMessageIDs,

 idNTPHeader = numOfDataMessageIDs,
 idNTPIdentifier,
 idNTPRequest,
 idNTPResponse,

 idReUseMe3, // to be removed
 idReUseMe4, // to be removed
 idReUseMe5, // to be removed
 idReUseMe6, // to be removed

 idDebugRequest,
 idDebugResponse,
 idDebugDataResponse,
 idDebugDataChangeRequest,
 idStreamSpecification,
 idModuleTable,
 idModuleRequest,
 idQueueFillRequest,
 idLogResponse,
 idDrawingManager,
 idDrawingManager3D,
 idDebugImage,
 idDebugJPEGImage,
 idDebugDrawing,
 idDebugDrawing3D,
 idText,
 idTypeInfo,
 idTypeInfoRequest,
 idPlot,
 idWorldPoseStatus,
 idRobotRegisterStatus,
 idEthernetIPFanuc,

});

