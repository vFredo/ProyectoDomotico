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
#define NUM_QUEUES 18 /* number of queues */

#define SISTEMA 0
// n + 3 = sensor_mov_casa_n y n + 6 = sensor_humo_casa_n
// casa 1 = 1
// casa 2 = 2
// casa 3 = 3

// sensor_mov_casa_1 = 4
// sensor_mov_casa_2 = 5
// sensor_mov_casa_3 = 6

// sensor_humo_casa_1 = 7
// sensor_humo_casa_2 = 8
// sensor_humo_casa_3 = 9

// Sensor_humedad_1 = 10
// Sensor_humedad_2 = 11
// Sensor_humedad_3 = 12

#define REGADO 13
#define SENSOR_FOTOELECTRICO 14
#define SENSOR_RFID 15
#define PANEL_ACCESO 16
#define SENSOR_HUMEDAD 17

/***( User-defined message structure )************************************/

typedef struct {
  int signal;
  double value;
  char placa[7];
  int sender;
} msg_t;

typedef struct {
  int idParent;
  int idSensor;
} parametros;

/***( User-defined signals)***********************************************/

typedef enum { sMovimiento, sExitMov } TO_SENSOR_MOVIMIENTO;

typedef enum { sNivelHumo, sExitHumo } TO_SENSOR_HUMO;

typedef enum { sNivelHumedad, sExitHumedad } TO_SENSOR_HUMEDAD;

typedef enum { sTarjetaEnSensor, sExitRFID } TO_SENSOR_RFID;

typedef enum { sLlegaCarro, sExitFotoElectrico } TO_SENSOR_FOTOELECTRICO;

typedef enum { sVisistanteSoliticaAcceso, sExitPanelAcceso } TO_PANEL_ACCESO;

typedef enum { sTiempoRegado, sExitRegado } TO_REGADO;

typedef enum {
  sNivelHumoCasa,
  sMovimientoCasa,
  sRoboDetenido,
  sNotificarCasa,
  sSeguridadActiva,
  sRespuestaResidente,
  sExitCasa
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
  sExitSistema
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
