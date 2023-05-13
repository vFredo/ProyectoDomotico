/**************************************************************************
 *
 *  pMLusrConf.h -   Manifest Constants and Types for concurrent access to
 *                   a circular buffer modelling a message queue
 *
 *   Notes:          User defined according to application
 *
 *************************************************************************/

/***( Manifest constants for fser-defined queuing system  )***************/

#define BUFSIZE 16   /* number of slots in queues */
#define NUM_QUEUES 9 /* number of queues */

#define CASA 0
#define SISTEMA 1
#define REGADO 2
#define SENSOR_MOVIMIENTO 3
#define SENSOR_HUMO 4
#define SENSOR_HUMEDAD 5
#define SENSOR_FOTOELECTRICO 6
#define SENSOR_RFID 7
#define PANEL_ACCESO 8

/***( User-defined message structure )************************************/

typedef struct {
  int signal;
  double value;
  char placa[7];
} msg_t;

/***( User-defined signals)***********************************************/

typedef enum { sMovimiento } TO_SENSOR_MOVIMIENTO;

typedef enum { sNivelHumo } TO_SENSOR_HUMO;

typedef enum { sNivelHumedad } TO_SENSOR_HUMEDAD;

typedef enum { sTarjetaEnSensor } TO_SENSOR_RFID;

typedef enum { sLlegaCarro } TO_SENSOR_FOTOELECTRICO;

typedef enum { sVisistanteSoliticaAcceso } TO_PANEL_ACCESO;

typedef enum { sTiempoRegado } TO_REGADO;

typedef enum {
  sNivelHumoCasa,
  sMovimientoCasa,
  sRoboDetenido,
  sNotificarCasa,
  sSeguridadActiva,
  sRespuestaResidente
} TO_CASA;

typedef enum {
  sLlegaPolicia,
  sLecturaCamara,
  sNivelHumedadSistema,
  sLlegaCarroSistema,
  sLecturaRFID,
  sIncendio,
  sRobo,
  sInvitadoAcepta,
  sSolicitud,
} TO_SISTEMA;

/***( User-defined EFSM states)*******************************************/

typedef enum { IdleMov } SENSOR_MOVIMIENTO_ESTADOS;

typedef enum { IdleHumo } SENSOR_HUMO_ESTADOS;

typedef enum { IdleHumedad } SENSOR_HUMEDAD_ESTADOS;

typedef enum { IdleFoto } SENSOR_FOTOELECTRICO_ESTADOS;

typedef enum { IdleRFID } SENSOR_RFID_ESTADOS;

typedef enum { IdlePanel } PANEL_ACCESO_ESTADOS;

typedef enum { IdleRegado } REGADO_ESTADOS;

typedef enum { IdleCasa, EsperandoRespuestaResidente } CASA_ESTADOS;

typedef enum {
  IdleSistema,
  EsperandoConfirmacion,
  EsperandoFoto,
  EsperandoPolicia,
  ValidandoTarjetaRFID
} SISTEMA_ESTADOS;
