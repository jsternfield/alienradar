#include "main.h"
#include "dithering.h"
#include "gpathbuilder.h"
#include <time.h>


static void render_bg(Layer* layer, GContext* ctx) 
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "render_bg");
    
    GPoint p0 = GPoint(SCREEN_CENTER_X, SCREEN_HEIGHT-CIRCLE_OFFSET);
    
#if PBL_COLOR
    draw_dithered_rect(ctx, GRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT),
                       darkColor, GColorBlack, DITHER_50_PERCENT);
    draw_dithered_circle(ctx, p0.x, p0.y, CIRCLE_RADIUS,
                             darkColor, GColorBlack, DITHER_10_PERCENT);
    draw_dithered_circle(ctx, p0.x, p0.y, CIRCLE_RADIUS/2,
                        darkColor, GColorBlack, DITHER_50_PERCENT);

    graphics_context_set_stroke_color(ctx, lightColor);
    graphics_context_set_fill_color(ctx, lightColor);
#else
    draw_dithered_circle(ctx, p0.x, p0.y, CIRCLE_RADIUS,
                         mTextColor, mBackgroundColor, DITHER_90_PERCENT);
    graphics_fill_circle(ctx, GPoint(p0.x, p0.y), CIRCLE_RADIUS/2);
    
    graphics_context_set_stroke_color(ctx, mTextColor);
    graphics_context_set_fill_color(ctx, mTextColor);
#endif // PBL_COLOR

    // Create GPathBuilder object
    GPathBuilder* builder = gpath_builder_create(MAX_POINTS);
    
    // Move to the starting point of the GPath
    for (int line = 0; line < NUM_LINES; ++line)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "render_bg path (line: %d)", line);
        gpath_builder_move_to_point(builder, p0);
        
        const int px = XOFFSET + line*14;
        const int py = GRIDOFFSET*2 + GRIDOFFSET*line;
        GPoint p1 = GPoint(px, py);
        gpath_builder_line_to_point(builder, p1);

        GPoint p2 = GPoint(SCREEN_WIDTH-px, py);
        
        const int cpy = py - GRIDOFFSET + line*4;
        GPoint cp1 = GPoint(SCREEN_WIDTH/3 + line*2, cpy);
        GPoint cp2 = GPoint(SCREEN_WIDTH/3*2 - line*2, cpy);
        gpath_builder_curve_to_point(builder, p2, cp1, cp2);
        
        gpath_builder_line_to_point(builder, p0);
    }
    
    // Create GPath object out of our GPathBuilder object
    s_path = gpath_builder_create_path(builder);
    
    // Display the path
    gpath_draw_outline(ctx, s_path);
    //gpath_draw_filled(ctx, s_path);

    gpath_builder_destroy(builder);
    
    // Draw the bisecting lines
    const int linex = (SCREEN_CENTER_X + XOFFSET) / 2;
    graphics_draw_line(ctx, p0, GPoint(linex, GRIDOFFSET));
    graphics_draw_line(ctx, p0, GPoint(SCREEN_CENTER_X, GRIDOFFSET));
    graphics_draw_line(ctx, p0, GPoint(SCREEN_WIDTH-linex, GRIDOFFSET));
    
    // Draw the black bars
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, GRect(0,0,XOFFSET/2+2,SCREEN_HEIGHT), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(SCREEN_WIDTH-XOFFSET/2-2,0,SCREEN_WIDTH,SCREEN_HEIGHT), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(0,SCREEN_HEIGHT-TEXT_SIZE,SCREEN_WIDTH,TEXT_SIZE), 0, GCornerNone);
    
    // Draw the side markers
#if PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorRed);
#else
    graphics_context_set_fill_color(ctx, mTextColor);
#endif // PBL_COLOR
    for (int bar = 0; bar < BAR_NUM; ++bar)
    {
        bool drawBar = (BAR_NUM-bar-1 < mBatteryLevel/10);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "BARS: num: %d, draw? %d", bar, drawBar);
        if (drawBar)
        {
            GRect barRect = GRect(0, (BAR_OFFSET+SCREEN_HEIGHT-TEXT_SIZE)/BAR_NUM*bar,
                                  XOFFSET/2, BAR_OFFSET/4);
            graphics_fill_rect(ctx, barRect, 0, GCornerNone);
        
            // Draw the other side
            barRect.origin.x = SCREEN_WIDTH - XOFFSET/2;
            graphics_fill_rect(ctx, barRect, 0, GCornerNone);
        }
    }
    
    // Draw dot for BT indicator.
    if (mIsBluetooth)
    {
        graphics_fill_circle(ctx, GPoint(SCREEN_CENTER_X+10, SCREEN_CENTER_X-14), 4);
    }
}

static void render_time(Layer* layer, GContext* ctx) 
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "render_time");
#ifdef PBL_COLOR
    graphics_context_set_text_color(ctx, GColorRed);
#else
    graphics_context_set_text_color(ctx, mTextColor);
#endif // PBL_COLOR

    // Draw the time.
    graphics_draw_text(ctx, mTimeText, mTimeFont,
                       GRect(TIMEX, TIMEY, TIME_SIZE*5, TIME_SIZE), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
    
    // Draw the time.
    graphics_draw_text(ctx, mDateText, mTimeFont,
                       GRect(DATEX, TIMEY, TIME_SIZE*8, TIME_SIZE), GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
}

static void set_time_display(struct tm* t)
{
    // Set the day
    int size = sizeof("00.00");
	if (mIs24HourStyle)
	{
		strftime(mTimeText, size, "%H:%M", t);
	}
	else
	{
		strftime(mTimeText, size, "%I:%M", t);
	}
    
    size = sizeof("00.00.00");
    strftime(mDateText, size, "%m-%d-%y", t);
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "set_time_display: %s (%s)", mTimeText, mDateText);
}

static void tick_handler(struct tm* t, TimeUnits units_changed)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "tick_handler");
	set_time_display(t);
    
    layer_mark_dirty(bgLayer);
    layer_mark_dirty(timeLayer);    
}

static void bt_handler(bool connected)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "bt_handler. connected: %d", connected);
    mIsBluetooth = connected;

    layer_mark_dirty(bgLayer);
}

static void battery_handler(BatteryChargeState charge_state)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "battery_handler. charging: %d, percent: %d", charge_state.is_charging, charge_state.charge_percent);
    
    mIsCharging = charge_state.is_charging;

    mBatteryLevel = charge_state.charge_percent;
    
    layer_mark_dirty(bgLayer);
}

static void window_load(Window *window)
{
    mBackgroundColor = GColorBlack;
    mTextColor = GColorWhite;

#if PBL_COLOR
    lightColor = GColorMintGreen;
    darkColor = GColorDarkGreen;
    
    window_set_background_color(window, darkColor);
#else
	window_set_background_color(window, mBackgroundColor);
#endif // PBL_COLOR

    bgLayer = layer_create(GRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
	layer_set_update_proc(bgLayer, (LayerUpdateProc)render_bg);
	layer_add_child(window_get_root_layer(window), bgLayer);
    
	timeLayer = layer_create(GRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
	layer_set_update_proc(timeLayer, (LayerUpdateProc)render_time);
	layer_add_child(window_get_root_layer(window), timeLayer);

    mTimeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BITWISE_16));
    mTimeText = malloc(5); // HH:MM
    mDateText = malloc(8); // MM-DD-YY

    struct tm* t;
	time_t temp;
	temp = time(NULL);
	t = localtime(&temp);
	set_time_display(t);
    
    bt_handler(bluetooth_connection_service_peek());
    battery_handler(battery_state_service_peek());
}

static void window_unload(Window *window) 
{
    app_timer_cancel(timer);
    //layer_destroy(timeLayer);
}

static void init(void)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init");
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) 
    {
		.load = window_load,
		.unload = window_unload,
    });
	
    APP_LOG(APP_LOG_LEVEL_DEBUG, "init: subscribe handlers");
    bluetooth_connection_service_subscribe(bt_handler);
    battery_state_service_subscribe(battery_handler);
    tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler)tick_handler);
    
    window_stack_push(window, true);
}

static void deinit(void) 
{
    tick_timer_service_unsubscribe();
    window_destroy(window);
}

int main(void) 
{
    init();
    app_event_loop();
    deinit();
}