// vim: sw=2 ts=2 noet :
/* ${license} */

#include <linux/notifier.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>

static struct input_dev *extkeys_input_device;
static int fn_state;

#define RK(key,state) input_event(extkeys_input_device,(key),(state)); return NOTIFY_STOP
int extkeys_transforming_filter (struct notifier_block *self,unsigned long value,void * data) 
{
	struct keboard_notifier_param * kp;
	//here's where the magick happens
	//currently there is no testing to see if events from the filter table collide with events filtered by it.
	if (value==KBD_KEYCODE)
	{
		kp=(keyboard_notifier_param *)data;
		switch(kp->value)
		{
			case KEY_LEFTCTRL:
				fn_state=kp->down;
				RK(KEY_FN);
			case KEY_LEFTMETA:
				RK(KEY_LEFTCTRL);
			case KEY_RIGHTALT:
				if (fn_state)
					RK(KEY_MENU);
				else
					RK(KEY_COMPOSE);
			default:
				if (fn_state)
				{
					switch(kp->value)
					{
						case KEY_POWER:
							RK(KEY_SYSRQ);
						case KEY_UP:
							RK(KEY_PAGEUP);
						case KEY_DN:
							RK(KEY_PAGEDOWN);
						case KEY_LEFT:
							RK(KEY_HOME);
						case KEY_RIGHT:
							RK(KEY_END);
					}
				}
		}
	}
	return NOTIFY_OK;
}

static struct notifer_block extkeys_transforming_filter_notifier = {
	.notifier_call = extkeys_transforming_filter,
}

#define HK(key) set_bit((key),extkeys_input_device.keybit)
static int __init extkeys_init(void)
{
	fn_state=0;
	extkeys_input_device=input_allocate_device();
	set_bit(EV_KEY,extkeys_input_device.evbit);
	HK(KEY_SYSRQ);
	HK(KEY_LEFTCTRL);
	HK(KEY_FN);
	HK(KEY_COMPOSE);
	HK(KEY_MENU);
	HK(KEY_HOME);
	HK(KEY_END);
	HK(KEY_PAGEUP);
	HK(KEY_PAGEDOWN);
	register_keyboard_notifier(&extkeys_transforming_filter_notifier);
	return 0;
}

static void __exit extkeys_exit(void)
{
	unregister_keyboard_notifier(&extkeys_transforming_filter_notifier);
	input_unregister_device(extkeys_input_device);
}

module_init(extkeys_init);
module_exit(extkeys_exit);

MODULE_LICENSE("GPL");
