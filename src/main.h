#pragma once

#include <pebble.h>
    
#define KEY_TWENTY_FOUR_HOUR_FORMAT 0
#define KEY_INVERT_COLORS 1

// CONSTS
#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168
#define SCREEN_CENTER_X 72
#define SCREEN_CENTER_Y 84

#define NUM_LINES 4
#define XOFFSET 20
#define YOFFSET 10
#define GRIDOFFSET 20
#define BAR_OFFSET 15
#define CIRCLE_OFFSET 50
#define CIRCLE_RADIUS 30

#define TIME_SIZE  16
#define TIMEX      TIME_SIZE
#define DATEX      -TIME_SIZE*3/2
#define TIMEY      SCREEN_HEIGHT - TIME_SIZE

#define BARX        12
#define BARY        32
#define BAR_HEIGHT  10
#define BAR_SEPARATOR 2
#define BAR_WIDTH   120
#define BAR_DIVISOR  7
#define BAR_MODIFIER 10
#define BAR_NUM     10

#define TEXT_SIZE 14
#define TOP_TEXT_X 0
#define TOP_TEXT_Y 0
#define BOT_TEXT_X 0
#define BOT_TEXT_Y (SCREEN_HEIGHT - TEXT_SIZE - 4)
#define TEXT_BAR_OFFSET_Y 2
#define LINE_OFFSET 4
    
#define MAX_POINTS 256
#define DRAW_LINE false
#define BENCHMARK false
#define MAX_DEMO_PATHS 4

struct Block
{
    int x;
    int y;
    int width;
    int height;
    bool visible;
};

static const int rot_step = TRIG_MAX_ANGLE / 360 * 5;
static GPath *s_path;
static uint8_t path_switcher = 0;
static bool draw_line_switcher = false;
#ifdef PBL_COLOR
static GColor8 foreground_color;
static GColor8 background_color;
#else
static GColor foreground_color;
static GColor background_color;
#endif

static Window* window;
static Layer* bgLayer;
static Layer* timeLayer;
static AppTimer* timer;
static GFont mDateFont;
static char* mDateText;
static GFont mTimeFont;
static char* mTimeText;
static char* mTopText;

struct Block* mBlocks[BAR_NUM];

GRect bgRect;

GColor mBackgroundColor;
GColor mTextColor;

#if PBL_COLOR
GColor lightColor;
GColor mediumColor;
GColor darkColor;
#endif // PBL_COLOR

int m_angle_tolerance = 30;

bool mIs24HourStyle = true;
bool mIsHorizontal = false;
bool mIsInverted = false;
bool mDrawDithers = true;
bool mDrawEllipse = true;
bool mIsCharging = false;
bool mIsBluetooth = false;
int mBatteryLevel = 100;