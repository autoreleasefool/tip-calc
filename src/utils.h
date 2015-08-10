#pragma once

extern int g_subtotal_cents;
extern int g_service_selection;
extern int g_tip_pct_great;
extern int g_tip_pct_avg;
extern int g_tip_pct_poor;

void load_tip_values(void);
void save_tip_values(void);
