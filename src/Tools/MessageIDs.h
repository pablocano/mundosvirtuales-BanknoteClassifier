/**
* @file MessageIDs.h
*
* Declaration of ids for debug messages.
*
* @author Martin LÃ¶tzsch
*/

#pragma once

#include "Tools/Enum.h"

/**
* IDs for debug messages
*
* To distinguish debug messages, they all have an id.
*/
ENUM(MessageID,
  undefined,
  idProcessBegin,
  idProcessFinished,

  // data (ids should remain constant over code changes, so old log files will still work)
  idImage,
  idJPEGImage,
  idFrameInfo,
  idFilteredJointData,
  idLinePercept,
  idGoalPercept,
  idBallPercept,
  idGroundTruthWorldState,
  idAudioData,
  idCameraMatrix,
  idCameraInfo,
  idImageCoordinateSystem,
  idMotionInfo,
  idRobotPose,
  idBallModel,
  idFilteredSensorData,
  idImageInfo,
  idOrientationData,
  idGameInfo,
  idRobotInfo,
  idOpponentTeamInfo,
  idSideConfidence,
  idDropInPlayer,
  idGroundTruthRobotPose,
  idGroundTruthBallModel,
  idGroundTruthOdometryData,
  idGroundTruthOrientationData,
  idColorCalibration,
  idOwnTeamInfo,
  idObstacleModel,
  idBehaviorControlOutput,
  idCombinedWorldModel,
  idRobotObstacles,
  idFieldBoundary,
  idRobotHealth,
  idActivationGraph,
  idThumbnail,
  idRobotPercept,
  idStopwatch,
  idLowFrameRateImage,
  idObstacleWheel,
  idBodyContour,
  idReceivedSPLStandardMessages,
  idLineSpots,
  idOdometer,
  idGroundContactState,
  idLocalizationTeamBall,
  idTeammateData,
  idTeammateDataCompressed,
  idTeammateReliability,
  // insert new data ids here
  idCurrentPositionConfidence,
  idCurrentSideConfidence,
  idAverageWalkSpeed,
  idMaxKickDistance,
  idXabslDebugSymbols,
  idXabslDebugMessage,
  idRoleAssignment,
  idBallDirection,
  idDataSPLStandardMessage,    //Test Pablo
  idGoalUnknownPercept,
  idMotionRequest,
  idPenaltyMarkPercept,
  idCameraRange,
  idArmMotionRequest,
  idHeadMotionRequest,
  idBehaviorLEDRequest,
  idBehaviorMotionControl,
  idBehaviorStatus,
  idStrikerDecision,
  idStrikerAvoidance,
  idNearBallTrajectory,
  idLogReUseMe1, // to be removed
  idLogReUseMe2, // to be removed
  idLogReUseMe3, // to be removed
  idLogReUseMe4, // to be removed
  idLogReUseMe5, // to be removed
  idLogReUseMe6, // to be removed
  numOfDataMessageIDs, /**< everything below this does not belong into log files */

  // ids used in team communication
  idNTPHeader = numOfDataMessageIDs,
  idNTPIdentifier,
  idNTPRequest,
  idNTPResponse,
  idRobot,
  idTeammateBallModel,
  idTeammateObstacleModel,
  idTeammateRobotPose,
  idTeammateSideConfidence,
  idTeammateBehaviorStatus,
  idTeammateGoalPercept,
  idTeammateIntention,
  idTeammateIsPenalized,
  idTeammateHasGroundContact,
  idTeammateIsUpright,
  idTeammateCombinedWorldModel,
  idWhistle,
  idRawGameInfo,
  idReUseMe3, // to be removed
  idReUseMe4, // to be removed
  idReUseMe5, // to be removed
  idReUseMe6, // to be removed
  idTeammateTimeSinceLastGroundContact,
  idTeamCameraHeight,
  idTeammateFieldCoverage,
  idReUseMe7,
  idWalkTarget,
  idKickTarget,
  idTeam,
  idTeammateBallAge,
  // insert new team comm ids here
  idTeammateRoleAssignment,
  idTeammateRobotObstacles,
  idTeammateStrikerDecision,
  // infrastructure
  idText,
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
  idXabslDebugRequest,
  idXabslIntermediateCode,
  idMotionNet,
  idJointRequest,
  idLEDRequest,
  idHandRequest,
  idPlot,
  idStaticPlot,
  idConsole,
  idRobotname,
  idRobotDimensions,
  idJointCalibration,
  idUSRequest,
  idWalkingEngineKick,
  idGroundTruthMessageRobot,
  idGroundTruthMessageBall,
  idEthernetIPFanuc
);

