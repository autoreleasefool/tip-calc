#pragma once

extern int s_subtotal_cents;
extern int s_service_selection;
extern int s_tip_pct_great;
extern int s_tip_pct_avg;
extern int s_tip_pct_poor;

void load_tip_values(void);
void save_tip_values(void);
