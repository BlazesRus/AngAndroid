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
import android.widget.Button;
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

class MiniKbd extends View
{
	final public GameActivity context;
	public StateManager state;

	public AdvButton(GameActivity p_context)
	{
		super(p_context);

		context = p_context;
		state = context.state;
	}

	public void setShiftMode(int mode)
	{
		/*
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
		*/
	}

	public void execute()
	{
		//InputUtils.processAction(state, action);

		//parent.setShiftMode(0);
	}

	public void configure(String action, boolean visible)
	{
	}

	public int calculateAlphaBg()
	{
		/*
		int min = 10;
		int max = 200;

		if (parent.opacityMode == 1 && !neverHidden()) {
			return min;
		}

		if (parent.opacityMode == 2) {
			return max;
		}

		// Normal
		return 80;
		*/
	}

	public int calculateAlphaFg()
	{
		/*
		int min = TermView.MIN_OPACITY;
		int max = 255;

		if (parent.opacityMode == 1 && !neverHidden()) {
			return min;
		}

		if (parent.opacityMode == 2) {
			return max;
		}

		int p = Preferences.getKeyboardOpacity();
		p = min + p * (max-min) / 100;

		if (atCenter()) {
			p = (p * Preferences.getMiddleOpacity()) / 100;
		}

		return Math.max(p, min);
		*/
	}

	public void setBgColor(Paint back)
	{
		/*
		if (pressed) {
			back.setColor(TOGGLED_BG);
			//back.setColor(DEFAULT_BG);
		}
		else if (defaultValue.equals(" ")) {
			back.setColor(TOGGLED_BG);
		}
		else if (usingKeymap()) {
			back.setColor(DEFAULT_BG);
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
		*/
	}

	public void setFgColor(Paint fore)
	{
		/*
		if (usingKeymap()) {
			fore.setColor(TOGGLED_BG);
		}
		else {
			fore.setColor(Color.WHITE);
		}
		*/
	}

	public void setPressed(boolean value)
	{
		/*
		if (value != pressed) {
			pressed = value;
			invalidate();
		}
		*/
	}

	public String getLabel()
	{
		/*
		String label = activeValue;
		if (usingKeymap() && Preferences.getShowAdvKeymaps()) {
			label = keymap;
		}
		return label;
		*/
		return "";
	}

	protected void onDraw(Canvas canvas)
	{
		Rect bounds = new Rect();

		getDrawingRect(bounds);

		/*
		Paint back = parent.back;
		Paint fore = parent.fore;

		back.setColor(Color.BLACK);
		back.setAlpha(10);
		canvas.drawRect(bounds, back);

		int pad = parent.getPad();

		bounds.top += pad;
		bounds.left += pad;
		bounds.bottom -= pad;
		bounds.right -= pad;

		String label = getLabel();
		if (label.length() > 3) label = label.substring(0, 3);
		if (label.length() == 1) fore = parent.foreBold;

		setBgColor(back);
		back.setAlpha(calculateAlphaBg());
		canvas.drawRect(bounds, back);

		--back.setColor(0xAAAAAA);
		--back.setAlpha(calculateAlphaBg());
		--canvas.drawLine(bounds.left, bounds.top, bounds.right, bounds.top, back);

		int tw = getWidth() - pad * 2;
		int th = getHeight() - pad * 2;

		float w2 = fore.measureText(label);
		float padx = Math.max((tw - w2) / 2, 0);
		float h2 = fore.descent() - fore.ascent();
		float pady = Math.max((th - h2) / 2, 0)	+ fore.descent();

		setFgColor(fore);
		fore.setShadowLayer(10f, 0, 0, Color.CYAN);
		fore.setAlpha(calculateAlphaFg());

		canvas.drawText(label,
			bounds.left + padx,
			bounds.top + th - pady,
			fore);
		*/
	}

	@Override
	protected void onMeasure(int widthMeasureSpec,
		int heightMeasureSpec)
	{
		/*
		int w = parent.btnWidth;
		int h = parent.btnHeight;

		setMeasuredDimension(w, h);
		*/
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		return true;
	}
}
