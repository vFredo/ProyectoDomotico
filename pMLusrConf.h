/*******************************************************************************
 *
 *  pMLusrConf.h -   Manifest Constants and Types for concurrent access to a
 *                   circular buffer modelling a message queue
 *
 *   Notes:          User defined according to application
 *
 *******************************************************************************/

/***( Manifest constants for fser-defined queuing system  )********************/

#define BUFSIZE 16    /* number of slots in queues */
#define NUM_QUEUES 10 /* number of queues */

#define CONTROLADOR_CASA 0
#define CONTROLADOR_SISTEMA 1

#define REGADO_ENV 2

#define SENSOR_MOVIMIENTO_ENV 3
#define SENSOR_HUMO_ENV 4
#define SENSOR_HUMEDAD_ENV 5
#define SENSOR_FOTOELECTRICO_ENV 6
#define SENSOR_RFID_ENV 7
#define PANEL_ACCESO_ENV 8
#define CONTROLADOR_CASA_ENV 9

/***( User-defined message structure )*****************************************/

typedef struct {
  int signal;
  double value;
} msg_t;

/***( User-defined signals)****************************************************/

typedef enum { sMovimiento } TO_SENSOR_MOVIMIENTO;

typedef enum { sNivelHumo } TO_SENSOR_HUMO;

typedef enum { sNivelHumedad } TO_SENSOR_HUMEDAD;

typedef enum { sTarjetaEnSensor } TO_SENSOR_RFID;

typedef enum { sLlegaCarro } TO_SENSOR_FOTOELECTRICO;

typedef enum {
  sNivelHumoCasa,
  sMovimientoCasa,
  sRoboDetenido,
  sNotificarCasa,
  sSeguridadActiva
} TO_CONTROLADOR_CASA;

typedef enum { sTiempoRegado } TO_REGADO;

typedef enum {
  sActivarAspersores,
  sDesactivarAspersores,
} FROM_REGADO;

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

typedef enum { sVisistanteSoliticaAcceso } TO_PANEL_ACCESO;

/***( User-defined EFSM states)************************************************/

typedef enum { IdleM } SENSOR_MOVIENTO_ESTADOS;

typedef enum { IdleC } CONTROLADOR_CASA_ESTADOS;

typedef enum { IdleH } SENSOR_HUMO_ESTADOS;

typedef enum { IdleHu } SENSOR_HUMEDAD_ESTADOS;

typedef enum { IdleF } SENSOR_FOTOELECTRICO_ESTADOS;

typedef enum {
  IdleS,
  EsperandoConfirmacion,
  EsperandoPeaton,
  EsperandoFoto,
  EsperandoCarro,
  EsperandoPolicia
} SISTEMA_ESTADOS;

typedef enum { IdleP } PANEL_ACCESO_ESTADOS;

typedef enum { IdleR } SENSOR_RFID_ESTADOS;

typedef enum { IdleRe, Regando } REGADO_ESTADOS;
