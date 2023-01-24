#define GenSectorBase					0x20
#define SectorLighting				0x1f
#define SectorDamage					0x60
#define SectorSecret					0x80
#define SectorIcy							0x100
#define SectorWindy						0x200

#define SectorLightingShift		0
#define SectorDamageShift			5
#define SectorSecretShift			7
#define SectorIcyShift				8
#define SectorWindyShift			9

#define GenFloorBase          0x6000
#define GenCeilingBase        0x4000
#define GenDoorBase           0x3c00
#define GenLockedBase         0x3800
#define GenLiftBase           0x3400
#define GenStairsBase         0x3000
#define GenCrusherBase        0x2F80

#define TriggerType	          0x0007
#define TriggerTypeShift      0

// define names for the TriggerType field of the general linedefs

typedef enum
{
	WalkOnce,
	WalkMany,
	SwitchOnce,
	SwitchMany,
	GunOnce,
	GunMany,
	PushOnce,
	PushMany,
} triggertype_e;

typedef struct buttonelt_s
{
	int idx;
	int row;
	int col;
	int grp;
	char **caparray;
	int *len;
	int *curval;
} ButtonElt;

typedef struct ButtonDialog_s
{
	int X0,Y0;
	int W,H;
	int BX0,BY0;
	int BW,BH;
	int BXS,BYS;
	int NR,NC;
	char **sections;
	char title[80];
	ButtonElt *btns;
}	ButtonDialog;

extern char *ParseGeneralizedLinedef(BCINT type);
extern char *ParseGeneralizedSectorType(BCINT type);
extern int ComputeFloorLinedefValue();
extern int ComputeCeilingLinedefValue();
extern int ComputeDoorLinedefValue();
extern int ComputeLockedLinedefValue();
extern int ComputeLiftLinedefValue();
extern int ComputeStairsLinedefValue();
extern int ComputeCrusherLinedefValue();
extern int ComputeGeneralizedSectorTypeValue();

extern int DisplayGeneralFloorDialog();
extern int DisplayGeneralCeilingDialog();
extern int DisplayGeneralDoorDialog();
extern int DisplayGeneralLockedDialog();
extern int DisplayGeneralLiftDialog();
extern int DisplayGeneralStairsDialog();
extern int DisplayGeneralCrusherDialog();
extern int DisplayGeneralSectorTypeDialog();

