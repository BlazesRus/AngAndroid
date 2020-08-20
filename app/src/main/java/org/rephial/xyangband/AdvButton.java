package org.rephial.xyangband;

import android.app.Activity;
import android.view.inputmethod.InputMethodManager;
import android.content.Context;
//import android.content.DialogInterface;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.graphics.Canvas;
import android.util.Log;
//import android.util.TypedValue;
import android.view.Gravity;
//import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
//import android.view.View.OnClickListener;
import android.view.GestureDetector;
import android.view.ViewGroup;
//import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
//import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.view.MotionEvent;
//import android.widget.PopupMenu;
import android.widget.PopupWindow;
//import android.widget.ScrollView;
//import android.widget.TableLayout;
import android.view.ViewGroup.LayoutParams;
//import android.widget.TableRow;
//import android.widget.TextView;
//import android.widget.HorizontalScrollView;
//import android.os.Handler;

import androidx.core.graphics.ColorUtils;

import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;

class AdvButton extends View
{
	final public GameActivity context;
	public StateManager state;	
	public String defaultValue;
	public String activeValue;	
	public AdvKeyboard parent;	
	public String keymap = "";
	public boolean alwaysVisible = false;
	public boolean keymapMode = false;
	public static int DEFAULT_BG = 0x294a59;
	public static int KEYMAP_BG = 0x7a3182;
	public static int TOGGLED_BG = 0x86bf36;
	public GestureDetector gestureManager = null;

	public AdvButton(GameActivity p_context, AdvKeyboard p_parent,
		String p_txt)
	{
		super(p_context);

		parent = p_parent;

		context = p_context;
		state = context.state;

		activeValue = defaultValue = p_txt;
	}

	public void setShiftMode(int mode)
	{
		if (usingKeymap()) return;

		if (defaultValue.length() == 1) {
			char chr = defaultValue.charAt(0);
			if (Character.isAlphabetic(chr)) {
				if (mode == 0) {
					activeValue = defaultValue;
				}
				if (mode == 1) {
					activeValue = ""+Character.toUpperCase(chr);
				}
				if (mode == 2) {
					activeValue = "^"+Character.toUpperCase(chr);
				}
				invalidate();
			}
		}
	}

	public boolean neverKeymap()
	{
		return defaultValue.equals(InputUtils.Visibility)
			|| defaultValue.equals(InputUtils.Enter)
			|| defaultValue.equals(InputUtils.Escape)
			|| defaultValue.equals(InputUtils.Shift)
			//|| defaultValue.equals(InputUtils.BackSpace)
			|| defaultValue.equals(" ")
			|| defaultValue.equals("sym")
			|| defaultValue.equals("abc")
			|| defaultValue.equals("kmp")
			|| defaultValue.equals(InputUtils.Menu);
	}

	public boolean neverHidden()
	{
		if (alwaysVisible) return true;

		return defaultValue.equals(InputUtils.Visibility)
			|| defaultValue.equals(InputUtils.Enter)
			|| defaultValue.equals(InputUtils.Escape)
			|| defaultValue.equals(" ")			
			//|| defaultValue.equals(".")
			//|| defaultValue.equals("run")
			//|| defaultValue.equals("sym")
			//|| defaultValue.equals("abc")
			|| defaultValue.equals("kmp")
			//|| defaultValue.equals(InputUtils.Menu)
			;
	}

	public boolean usingKeymap()
	{
		return (keymapMode && keymap.length() > 0);
	}

	public void execute()
	{
		String action = activeValue;

		if (action.equals("kmp")) {
			parent.toggleKeymapMode();
			return;
		}

		if (parent.opacityMode == 1 && !neverHidden()) {
			parent.setOpacityMode(0);
			return;
		}

		if (usingKeymap()) {
			action = keymap;
		}
		else {
			char code = InputUtils.codeFromName(action);
			if (code > 0) {
				state.addKey(code);
				parent.setShiftMode(0);
				return;
			}
		}

		if (action.equals(InputUtils.Visibility)) {
			parent.changeOpacityMode();
			return;
		}

		if (action.equals(InputUtils.Shift)) {
			parent.changeShiftMode();
			return;
		}

		if (action.equals(InputUtils.Menu)) {
			context.handler.sendEmptyMessage(AngbandDialog.Action.
                OpenContextMenu.ordinal());
			return;
		}

		if (action.equals("sym") || action.equals("abc")) {
			parent.changePage();
			return;
		}

		if (action.equals("run")) {
			state.setRunningMode(!state.getRunningMode());
			invalidate();
			return;
		}

		List<Integer> list = InputUtils.parseCodeKeys(action);
		for (Integer keycode: list) {
			if (keycode > 0) {
				state.addKey(keycode);			
			}
		}
		parent.setShiftMode(0);	
	}

	public void setBgColor(Paint back)
	{
		if (usingKeymap()) {
			back.setColor(KEYMAP_BG);
		}
		else if (defaultValue.equals(" ")) {
			back.setColor(TOGGLED_BG);	
		}
		else if (defaultValue.equals("kmp") && keymapMode) {
			back.setColor(TOGGLED_BG);	
		}
		else if (defaultValue.equals("run")
			&& state.getRunningMode()) {
			back.setColor(TOGGLED_BG);	
		}
		else {
			back.setColor(DEFAULT_BG);
		}
	}

	public static void closeSoftKeyboard(Activity ctxt, View v)
    {        
        InputMethodManager imm = (InputMethodManager)ctxt
            .getSystemService(Activity.INPUT_METHOD_SERVICE);        
        imm.hideSoftInputFromWindow(v.getWindowToken(), 0);        
    }

    public void configure(String action, boolean visible)
    {
    	if (!action.equals(keymap) ||
    		visible != alwaysVisible) {
			keymap = action;
			alwaysVisible = visible;
			invalidate();
			parent.updateKey(defaultValue, action, visible);
			parent.persistKeymaps();
		}
    }

    public class OptionPopup extends PopupWindow
    {
    	public EditText etext = null;
    	public CheckBox ckAlwaysVisible = null;

    	public OptionPopup()
    	{
    		super(context);

    		setFocusable(true);
        	setWidth(LayoutParams.WRAP_CONTENT);
        	setHeight(LayoutParams.WRAP_CONTENT);

        	LinearLayout content = (LinearLayout)
        		context.getLayoutInflater().inflate
            	(R.layout.adv_button_options, null);

        	etext = (EditText)content.findViewById
        		(R.id.keymap_action);

        	etext.setText(keymap);
			etext.setSelection(etext.getText().length());

			ckAlwaysVisible = (CheckBox)content.findViewById
        		(R.id.always_visible);
        	ckAlwaysVisible.setChecked(alwaysVisible);

        	setContentView(content);
    	}

    	@Override
    	public void dismiss()
    	{
    		String action = etext.getText().toString().trim();

    		closeSoftKeyboard(context, etext);        		

    		configure(action, ckAlwaysVisible.isChecked());

    		super.dismiss();
    	}
    }

	public void showOptions()
	{
		OptionPopup win = new OptionPopup();        

        int gravity = Gravity.TOP
        	| Gravity.CENTER_HORIZONTAL;
        win.showAtLocation(this, gravity, 0, 10);
	}

	public boolean atCenter()
	{
		int[] loc = new int[2];
		getLocationInWindow(loc);
		int x = loc[0];

		float pad_pct = 0.33f;
		int midx = x + getWidth() / 2;
		float pct = (float)midx / Math.max(1, parent.winSize.x);
		boolean at_center = false;
		if (pct > pad_pct && pct < (1.0f - pad_pct)) {
			at_center = true;
		}
		
		return at_center;
	}

	public int calculateAlphaBg()
	{
		if (parent.opacityMode == 1 && !neverHidden()) {
			return 10;
		}

		int max = 220;

		if (parent.opacityMode == 2) {
			return max;
		}

		int p = Preferences.getKeyboardOpacity();
		p = (max * p) / 100;

		if (atCenter()) {
			p = (p * Preferences.getMiddleOpacity()) / 100;
		}

		return Math.max(p, 10);
	}

	public int calculateAlphaFg()
	{
		if (parent.opacityMode == 1 && !neverHidden()) {
			return 30;
		}

		int max = 255;

		if (parent.opacityMode == 2) {
			return max;
		}

		int p = Preferences.getKeyboardOpacity();
		p = (max * p) / 100;

		if (atCenter()) {
			p = (p * Preferences.getMiddleOpacity()) / 100;
		}

		return Math.max(p, 10);
	}
	
	protected void onDraw(Canvas canvas)
	{
		Rect bounds = new Rect();

		getDrawingRect(bounds);

		Paint back = parent.back;
		Paint fore = parent.fore;

		back.setColor(Color.BLACK);
		back.setAlpha(10);
		canvas.drawRect(bounds, back);

		int pad = 2;

		bounds.top += pad;
		bounds.left += pad;
		bounds.bottom -= pad;
		bounds.right -= pad;

		String label = activeValue;
		if (usingKeymap()) label = keymap;
		if (label.length() > 3) label = label.substring(0, 3);

		if (label.length() == 1) fore = parent.foreBold;

		setBgColor(back);
		back.setAlpha(calculateAlphaBg());

		canvas.drawRect(bounds, back);

		int tw = getWidth() - pad * 2;
		int th = getHeight() - pad * 2;

		float w2 = fore.measureText(label);
        float padx = Math.max((tw - w2) / 2, 0);
        float h2 = fore.descent() - fore.ascent();
        float pady = Math.max((th - h2) / 2, 0)	+ fore.descent();        

        fore.setColor(Color.WHITE);
        fore.setShadowLayer(10f, 0, 0, Color.CYAN);
        fore.setAlpha(calculateAlphaFg());

		canvas.drawText(label,
			bounds.left + padx,
			bounds.top + th - pady,
			fore);
	}

	@Override
	protected void onMeasure(int widthMeasureSpec,
		int heightMeasureSpec)
	{
		int w = parent.btnWidth;
		int h = parent.btnHeight;

		setMeasuredDimension(w, h);
	}

	public GestureDetector getGestureManager()
	{
		if (gestureManager != null) return gestureManager;

		gestureManager = new GestureDetector(
			new GestureDetector.SimpleOnGestureListener() 
		{
			@Override
			public boolean onDown(MotionEvent event) {
				return true;
			}

			@Override
			public boolean onSingleTapUp(MotionEvent event) {
				execute();
				return true;
			}

			@Override
			public void onLongPress(MotionEvent event) {

				if (defaultValue.equals(InputUtils.Visibility)) {
					parent.setOpacityMode(2);
					return;
				}

				if (!neverKeymap()) {
					showOptions();
				}
			}
		});

		return gestureManager;		
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {

		return getGestureManager().onTouchEvent(event);
	}
}
