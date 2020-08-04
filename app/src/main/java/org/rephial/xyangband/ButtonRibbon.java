package org.rephial.xyangband;

import org.rephial.xyangband.StateManager.CoreCommand;

import android.content.DialogInterface;
import android.graphics.Color;
import android.graphics.Point;
import android.graphics.Typeface;
import android.util.Log;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.PopupMenu;
import android.widget.PopupWindow;
import android.widget.ScrollView;
import android.widget.TableLayout;
import android.view.ViewGroup.LayoutParams;
import android.widget.TableRow;
import android.widget.TextView;
import android.os.Handler;

import androidx.core.graphics.ColorUtils;

import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;

public class ButtonRibbon implements OnClickListener,
        PopupMenu.OnMenuItemClickListener {

    static int count = 1;
    static Typeface fontCmd = null;

    View rootView = null;
    GameActivity context = null;
    StateManager state = null;
    LinearLayout atLeft = null;
    LinearLayout atCenter = null;
    LinearLayout atRight = null;
    boolean fastMode = false;
    int fixedCount = 0;
    ArrayList<Command> commands = null;
    boolean shifted = false;
    boolean controlDown = false;
    boolean commandMode = false;
    int alphaLevel = 2;
    ArrayList<ButtonRibbon> siblings = null;

    PopupWindow autoListWin = null;
    TableLayout autoListTable = null;    
    boolean autoListSolid = false;

    public static char KC_TAB = 0x9D;
    public static char KC_BACKSPACE = 0x9F;

    public static char KC_ARROW_DOWN = 0x80;
    public static char KC_ARROW_LEFT = 0x81;
    public static char KC_ARROW_RIGHT = 0x82;
    public static char KC_ARROW_UP = 0x83;

    public static char KC_F1 = 0x84;
    public static char KC_F2 = 0x85;
    public static char KC_F3 = 0x86;
    public static char KC_F4 = 0x87;
    public static char KC_F5 = 0x88;
    public static char KC_F6 = 0x89;
    public static char KC_F7 = 0x8A;
    public static char KC_F8 = 0x8B;
    public static char KC_F9 = 0x8C;
    public static char KC_F10 = 0x8D;
    public static char KC_F11 = 0x8E;
    public static char KC_F12 = 0x8F;

    public static char keycodes[] = {
            KC_TAB,
            KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6,
            KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
            0
    };

    public static String keynames[] = {
            "tab",
            "F1", "F2", "F3", "F4", "F5", "F6",
            "F7", "F8", "F9", "F10", "F11", "F12",
            ""
    };

    public static int popupBackAlpha = 255;
    public static int popupButtonColor = Color.BLACK;

    public String userKeymaps = "";

    public enum CmdLocation {
        Fixed,
        Dynamic,
        Keymaps
    }

    public static char KTRL(char c)
    {
        return (char)(c & 0x1F);
    }

    public static char UN_KTRL(char c)
    {
        if (c < 0x01 || c > 0x1F) {
            return c;
        }
        return (char)(c + 64);
    }

    public static boolean isKtrl(String str)
    {
        return str.length() == 2 && str.charAt(0) == '^' &&
                Character.isAlphabetic(str.charAt(1));
    }

    public static String printableChar(char c)
    {
        if (c >= 1 && c <= 0x1F) {
            return "^" + UN_KTRL(c);
        }

        for (int i = 0; keycodes[i] != 0; i++) {
            if (keycodes[i] == c) {
                return keynames[i];
            }
        }

        return Character.toString(c);
    }

    public static char getKeyCode(String printable)
    {
        if (printable.length() == 0) {
            return 0;
        }

        int i = 0;
        for (String name: keynames) {
            if (name.equals(printable)) {
                return keycodes[i];
            }
            i++;
        }

        if (isKtrl(printable)) {
            return KTRL(printable.charAt(1));
        }

        return printable.charAt(0);
    }

    public class Command implements Button.OnLongClickListener {
        public String label;
        public String action;
        public char charValue;
        public boolean isAlpha;
        public CmdLocation location;
        public Button btn;

        public Command(String p_label, String p_action,
               CmdLocation p_location, Button p_btn)
        {
            label = p_label;
            action = p_action;
            charValue = 0;
            location = p_location;
            btn = p_btn;
            isAlpha = false;

            // Single character
            if (label.length() == 1) {
                charValue = label.charAt(0);
                label = printableChar(charValue);
            }

            if (action.length() == 0) {
                action = label;
            }

            setButtonText(label);

            // Just a simple alphabetic character
            if (Character.isAlphabetic(charValue) && action == label) {
                isAlpha = true;
            }
        }

        public void setControlDown(boolean set)
        {
            String text;

            if (isAlpha) {
                if (set) {
                    text = "^" + label.toUpperCase();
                    action = text;
                }
                else {
                    action = text = label;
                }
                setButtonText(text);
                btn.invalidate();
            }
        }

        public void setShift(boolean set) {
            // Lower letter ?
            if (isAlpha) {
                if (Character.isUpperCase(charValue)) {
                    set = !set;
                }
                if (set) {
                    action = label.toUpperCase();
                }
                else {
                    action = label.toLowerCase();
                }
                setButtonText(action);
                btn.invalidate();
            }
        }

        public boolean isCommand()
        {
            return action.length() > 4 && action.startsWith("CMD_");
        }

        public char getCommand(boolean translate)
        {
            if (isCommand()) {
                char cmd = action.charAt(4);

                if (translate) {
                    // Original keyset
                    CoreCommand core = state.findCommand(cmd, 0);
                    if (core != null) {
                        // Convert, or not
                        cmd = (char)core.getKey();
                    }
                }

                return cmd;
            }
            return 0;
        }

        public char fixIcon()
        {
            char raw = getCommand(false);
            if (raw == 'h') raw = 0xF9;
            if (raw == 'U') raw = 0xF5;
            if (raw == 'n') raw = 0xD2;
            if (raw == '~') raw = 0xEF;
            if (raw == '+') raw = 0x42;
            return raw;
        }

        public void setIconTooltip()
        {
            // Set tooltip text
            if (android.os.Build.VERSION.SDK_INT >= 26) {
                char cmd = getCommand(true);
                btn.setTooltipText(printableChar(cmd));
            }
        }

        public void setButtonText(String txt)
        {
            // Set icon
            if (isCommand()) {
                if (Preferences.getIconsEnabled() &&
                        ButtonRibbon.fontCmd != null) {
                    btn.setTypeface(ButtonRibbon.fontCmd);
                    //setIconTooltip();
                    btn.setOnLongClickListener(this);
                    txt = Character.toString(fixIcon());
                    btn.setText(txt);
                    // Done!
                    return;
                }
                else {
                    txt = printableChar(getCommand(true));
                    // Keep going
                }
            }



            // Set bold
            if (txt.length() == 1) {
                btn.setTypeface(context.monoBoldFont);
                /*
                SpannableString spanString = new SpannableString(txt);
                spanString.setSpan(new StyleSpan(Typeface.BOLD),
                        0, spanString.length(), 0);
                btn.setText(spanString);
                */
                btn.setText(txt);
            }
            // Normal text
            else {
                btn.setTypeface(context.monoFont);
                btn.setText(String.format("%.3s", txt));
            }
        }

        public boolean isKeymap()
        {
            return action.startsWith("KEYMAP_");
        }

        public boolean isUserCommand()
        {
            return action.startsWith("USER_");
        }

        public int getTrigger()
        {
            try {
                String code = action.substring("KEYMAP_".length());
                return Integer.parseInt(code);
            } catch (Exception ex) {
                return 0;
            }
        }

        @Override
        public boolean onLongClick(View v) {

            char cmd = getCommand(false);
            String desc = "";

            CoreCommand core = state.findCommand(cmd, 0);
            if (core != null) {
                cmd = (char)core.getKey();
                desc = core.desc;
            }

            String txt = printableChar(cmd);

            if (desc == null) desc = "";
            String msg = "Execute command: " + txt + "\n\n" + desc;
            context.questionAlert(msg,
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                            btn.performClick();
                        }
                    });
            return false;
        }
    }

    public ButtonRibbon(GameActivity p_context, StateManager p_state,
                        boolean p_fastMode) {
        context = p_context;
        state = p_state;
        fastMode = p_fastMode;

        rootView = context.getLayoutInflater().inflate(R.layout.buttonribbon, null);

        atLeft = rootView.findViewById(R.id.at_left);
        atCenter = rootView.findViewById(R.id.at_center);
        atRight = rootView.findViewById(R.id.at_right);

        commands = new ArrayList();
        siblings = new ArrayList();

        alphaLevel = Preferences.getRibbonAlpha();

        if (fontCmd == null) {
            try {
                fontCmd = Typeface.createFromAsset(context.getAssets(),"ui-cmd.ttf");
            } catch (Exception ex) {
                Log.d("Angband", "Can't load cmd font");
                fontCmd = null;
            }
        }

        String userActions[] = KeymapEditor.getUserKeymaps();

        // Fixed keys
        if (fastMode) {
            makeCommand("⎋", "Esc", CmdLocation.Fixed);
            makeCommand("⏎", "Ret", CmdLocation.Fixed);
            makeCommand("⌫", "BackSpace", CmdLocation.Fixed);
            makeCommand("⎘", "show_keys", CmdLocation.Fixed);

            userKeymaps = userActions[0];

            rebuildKeymaps();
        }
        else {
            makeCommand("▤", "Kbd", CmdLocation.Fixed);
            makeCommand("✦", "do_cmd_list", CmdLocation.Fixed);
            makeCommand("⎘", "show_keys", CmdLocation.Fixed);
            makeCommand("⇧", "Sft", CmdLocation.Fixed);
            makeCommand("^", "Ctrl", CmdLocation.Fixed);

            userKeymaps = userActions[1];

            if (state.inTheDungeon()) { 
                restoreCommandMode();
            }
            else {
                setCommandMode(false);
            }
        }
    }

    public void addSibling(ButtonRibbon ribbon)
    {
        siblings.add(ribbon);
    }

    public void restoreCommandMode()
    {
        setCommandMode(Preferences.getCommandMode());
    }

    public void setCommandMode(boolean set)
    {
        if (fastMode) {
            return;
        }

        // Already created and equal, do nothing
        if (atCenter.getChildCount() > 0 && set == commandMode) {
            return;
        }

        removeCommands(atCenter);

        rebuildUserKeymaps(CmdLocation.Dynamic);

        if (set) {
            // Hide shift and ctrl
            atLeft.getChildAt(3).setVisibility(View.GONE);
            atLeft.getChildAt(4).setVisibility(View.GONE);

            String txt = ".iUmhfvngdR+ewb,azul[]C~LM=?";
            for (char c: txt.toCharArray()) {
                String label = Character.toString(c);
                String action = "CMD_" + c;
                makeCommand(label, action, CmdLocation.Dynamic);
            }
        }
        else {
            // Show shift and ctrl
            atLeft.getChildAt(3).setVisibility(View.VISIBLE);
            atLeft.getChildAt(4).setVisibility(View.VISIBLE);

            setKeys("abcdefghijklmnopqrstuvwxyz " +
                    "0123456789.,*'~!#$%&<>|" +
                    "/\\=[]{}@+-_:;\"?", CmdLocation.Dynamic);
            // Other keys
            for (int i = 0; keynames[i].length() > 0; i++) {
                makeCommand(keynames[i], Character.toString(keycodes[i]),
                        CmdLocation.Dynamic);
            }
        }

        atLeft.requestLayout();
        atLeft.invalidate();
        atCenter.invalidate();

        commandMode = set;
        shifted = false;
        controlDown = false;
    }

    public void resizeButton(Button btn)
    {
        int mult = Preferences.getRibbonButtonMult();

        int w = btn.getMinWidth();

        if (mult < 45 || mult > 55) {

            float pct = 0.5f + (mult / 100f);
            pct = Math.min(pct, 1.5f);
            pct = Math.max(pct, 0.5f);

            w = (int)(w * pct);
            // Hack, set both to get it working
            btn.setMinimumWidth(w);
            btn.setMinWidth(w);

            float fw = btn.getTextSize();        
            btn.setTextSize(TypedValue.COMPLEX_UNIT_PX, fw * pct);
        }
    }

    public Button makeButton(String text, String action) {
        Button btn = (Button)context.getLayoutInflater().inflate
                (R.layout.aribbonbutton, null);

        btn.setText(text);
        btn.setOnClickListener(this);  

        resizeButton(btn);

        if (action.equals("Ret")) {            
            int w = btn.getMinWidth();
            w = (int)(w * 1.5f);
            btn.setWidth(w);
        }

        return btn;
    }

    public void removeCommands(ViewGroup parent)
    {
        if (parent.getChildCount() == 0) {
            return;
        }

        for (int i = 0; i < parent.getChildCount(); i++) {
            Button btn = (Button)parent.getChildAt(i);
            Command cmd = findCommand(btn);
            commands.remove(cmd);
        }

        parent.removeAllViews();
        parent.invalidate();
    }

    public void setShift(boolean set)
    {
        if (fastMode || commandMode ||
                (set == shifted && !controlDown)) {
            return;
        }

        for (Command cmd: commands) {
            cmd.setShift(set);
        }
        shifted = set;
        controlDown = false;
    }

    public void setControlDown(boolean set)
    {
        if (fastMode || commandMode ||
                (set == controlDown && !shifted)) {
            return;
        }

        for (Command cmd: commands) {
            cmd.setControlDown(set);
        }
        controlDown = set;
        shifted = false;
    }

    public void makeCommand(String label, String action, CmdLocation loc)
    {
        Button btn = makeButton(label, action);
        Command cmd = new Command(label, action, loc, btn);
        commands.add(cmd);
        updateAlphaCmd(cmd);
        if (loc == CmdLocation.Fixed) {
            atLeft.addView(btn);
        }
        else if (loc == CmdLocation.Keymaps) {
            atRight.addView(btn);
        }
        else {
            atCenter.addView(btn);
        }
    }

    public void removeAutoList()
    {
        Log.d("Angband", "Remove Auto List");

        if (autoListWin != null) {
            autoListWin.dismiss();
        }

        autoListWin = null;
        autoListTable = null;
        autoListSolid = false;
    }

    public void clearFastKeys() {
        if (fastMode) {
            removeCommands(atCenter);

            removeAutoList();       
        }
    }

    public void rebuildUserKeymaps(CmdLocation loc)
    {
        String keymaps[] = userKeymaps.split("###");
        //Arrays.sort(keymaps);
        for (String str: keymaps) {

            if (str.length() == 0) continue;

            String label = str.replace("\\s", "")
                    .replace("\\S", "")
                    .replace("\\e", "")
                    .replace("\\E", "")
                    .replace("\\n", "")
                    .replace("\\N", "");

            if (label.length() > 3) label = str.substring(0, 3);
            String action = "USER_" + str;
            makeCommand(label, action, loc);
        }
    }

    public void rebuildKeymaps()
    {
        removeCommands(atRight);
        atRight.invalidate();

        rebuildUserKeymaps(CmdLocation.Keymaps);

        String coreKeymaps = Preferences.getCoreKeymaps();
        if (coreKeymaps.length() > 0) {

            String keymaps[] = coreKeymaps.split("###");
            Arrays.sort(keymaps);
            for (String str: keymaps) {
                String parts[] = str.split("##");
                if (parts.length != 2) {
                    continue;
                }
                String label = parts[0];
                String action = "KEYMAP_" + parts[1];
                makeCommand(label, action, CmdLocation.Keymaps);
            }
        }
    }

    public void addFKeys(CmdLocation loc)
    {
        for (int i = 0; keynames[i].length() > 0; i++) {
            if (keycodes[i] >= KC_F1 && keycodes[i] <= KC_F12) {
                makeCommand(keynames[i], Character.toString(keycodes[i]),
                        loc);
            }
        }
    }

    public void setKeys(String keys, CmdLocation loc) {

        boolean showList = false;

        clearFastKeys();

        if (fastMode && loc == CmdLocation.Dynamic) {

            if (keys.length() > 0) {

                if (keys.length() > 6) {
                    showList = true;
                }

                removeCommands(atRight);

                String pattern = "[^fkeys$]";
                if (keys.contains(pattern)) {
                    addFKeys(loc);
                    // Remove the pattern
                    keys = keys.replace(pattern, "");
                }
            }
            else {
                if (atRight.getChildCount() == 0) {
                    rebuildKeymaps();
                }
            }            
        }

        for (char c: keys.toCharArray()) {
            String label = Character.toString(c);
            makeCommand(label, "", loc);
        }

        if (Preferences.getEnableTouch() && !Preferences.getTouchRight()) {
            showList = false;
        }

        if (!Preferences.getShowAutoList()) {
            showList = false;
        }

        if (showList) {
            showAutoList(rootView);
        }
    }

    public Command findCommand(Button btn)
    {
        for (Command cmd: commands) {
            if (cmd.btn == btn) {
                return cmd;
            }
        }
        return null;
    }

    public void showMenu()
    {
        PopupMenu popup = new PopupMenu(context, rootView);

        popup.inflate(R.menu.ribbon);
        popup.setOnMenuItemClickListener(this);
        popup.show();
    }

    public void updateAlphaCmd(Command cmd)
    {
        int alphaBg = 50;
        int alphaFg = (255 * Preferences.getKeyboardOpacity()) / 100;
        int color = Color.WHITE;

        if (cmd.action.equals("Ret")) {
            alphaBg = 100;
        }

        if (alphaLevel == 1) {
            alphaFg = 30;
            alphaBg = 10;            
        }
        if (alphaLevel == 3) {
            color = 0x000044;
            alphaFg = 255;
            alphaBg = 200;
        }

        color = ColorUtils.setAlphaComponent(color, alphaFg);
        cmd.btn.setTextColor(color);
        cmd.btn.getBackground().setAlpha(alphaBg);
    }

    public void updateAlpha()
    {
        Preferences.setRibbonAlpha(alphaLevel);

        for (Command cmd: commands) {
            updateAlphaCmd(cmd);
            cmd.btn.invalidate();
        }

        for (ButtonRibbon ribbon: siblings) {
            ribbon.alphaLevel = this.alphaLevel;
            for (Command cmd: ribbon.commands) {
                updateAlphaCmd(cmd);
                cmd.btn.invalidate();
            }
        }
    }

    public void toggleCommandMode()
    {
        setCommandMode(!commandMode);
        Preferences.setCommandMode(commandMode);
    }

    public void changeOpacity(int step)
    {
        alphaLevel = alphaLevel + step;
        if (alphaLevel < 1) alphaLevel = 2;
        if (alphaLevel > 3) alphaLevel = 2;
        updateAlpha();
    }

    @Override
    public boolean onMenuItemClick(MenuItem item) {
        switch(item.getItemId()) {
            case (R.id.toggle_ribbon):
                toggleCommandMode();
                return true;
            case (R.id.show_keyboard):
                context.toggleKeyboard();
                return true;
            case (R.id.lower_opacity):
                changeOpacity(-1);
                return true;
            case (R.id.raise_opacity):
                changeOpacity(+1);
                return true;
            case (R.id.show_pref):
                context.handler.sendEmptyMessage(AngbandDialog.Action.
                        OpenContextMenu.ordinal());
                return true;
        }

        return false;
    }

    public void showCommandList(View parentView) {

        // Hack
        if (state.coreCommands.isEmpty()) {
            state.buildCommandList();
        }

        final PopupWindow win = new PopupWindow(context);
        win.setFocusable(true);
        win.setWidth(LayoutParams.WRAP_CONTENT);
        win.setHeight(LayoutParams.WRAP_CONTENT);

        ScrollView scroll = new ScrollView(context);
        scroll.setLayoutParams(new LayoutParams(
                LayoutParams.WRAP_CONTENT,
                LayoutParams.MATCH_PARENT
        ));

        win.setContentView(scroll);

        TableLayout table = new TableLayout(context);
        //table.setShrinkAllColumns(true);
        table.setLayoutParams(new LayoutParams(
                LayoutParams.WRAP_CONTENT,
                LayoutParams.WRAP_CONTENT
        ));

        scroll.addView(table);

        TableRow trow = null;

        int maxRowItems = 4;
        /*
        if (context.landscapeNow()) {
            maxRowItems = 6;
        }
        */

        OnClickListener clickListener =
            new OnClickListener() {
                @Override
                public void onClick(View v) {
                    Integer tag = (Integer)v.getTag();
                    int key = tag.intValue();
                    state.addKey(key);
                    win.dismiss();
                }
            };

        for (CoreCommand cmd: state.coreCommands) {
            int key = cmd.getKey();
            String trigger = printableChar((char)key);
            String desc = cmd.desc;

            //Log.d("Angband", trigger + " | " + desc);

            if (trow == null || trow.getChildCount() == maxRowItems) {
                trow = new TableRow(context);
                trow.setLayoutParams(new LayoutParams(
                        LayoutParams.WRAP_CONTENT,
                        LayoutParams.WRAP_CONTENT
                ));
                table.addView(trow);
            }

            Button btn = (Button)context.getLayoutInflater().inflate
                    (R.layout.popupbutton, null);
            //Button btn = new Button(context);
            btn.setText(trigger);
            btn.setTag(new Integer(key));
            btn.setOnClickListener(clickListener);
            btn.setTypeface(context.monoBoldFont);
            btn.getBackground().setAlpha(popupBackAlpha);
            btn.setTextColor(popupButtonColor);
            trow.addView(btn);

            TextView txt = new TextView(context);
            txt.setText(desc);
            txt.setClickable(true);
            txt.setTag(new Integer(key));
            txt.setOnClickListener(clickListener);
            trow.addView(txt);
        }

        if (trow != null && trow.getChildCount() == 0) {
            table.removeView(trow);
        }

        if (table.getChildCount() == 0) return;

        //win.showAsDropDown(parentView);
        //win.update(0, 0, win.getWidth(), win.getHeight());
        win.showAtLocation(parentView,
                Gravity.LEFT | Gravity.TOP,
                0,0);
    }

    public void showDynamicKeys(View parentView) {
        CmdLocation loc = CmdLocation.Dynamic;
        if (atCenter.getChildCount() == 0) {
            if (atRight.getChildCount() == 0) {
                return;
            }
            loc = CmdLocation.Keymaps;
        }

        final PopupWindow win = new PopupWindow(context);        
        win.setFocusable(true);  
        win.setWidth(LayoutParams.WRAP_CONTENT);
        win.setHeight(LayoutParams.WRAP_CONTENT);

        ScrollView scroll = new ScrollView(context);
        scroll.setLayoutParams(new LayoutParams(
                LayoutParams.WRAP_CONTENT,
                LayoutParams.MATCH_PARENT
        ));

        win.setContentView(scroll);

        TableLayout table = new TableLayout(context);
        table.setLayoutParams(new LayoutParams(
                LayoutParams.WRAP_CONTENT,
                LayoutParams.WRAP_CONTENT
        ));

        scroll.addView(table);

        TableRow trow = null;

        int maxRowItems = 3;
        if (context.landscapeNow()) {
            maxRowItems = 6;            
        }

        OnClickListener clickListener =
            new OnClickListener() {
                @Override
                public void onClick(View v) {
                    Integer tag = (Integer)v.getTag();
                    int savedIdx = tag.intValue();
                    // Indirection
                    Command cmd = commands.get(savedIdx);
                    if (cmd != null) cmd.btn.performClick();                    
                    win.dismiss();                    
                }
            };

        int idx = -1;
        int alphaFg = 220;
        int alphaBg = 50;

        for (Command cmd: this.commands) {
            ++idx;

            if (cmd.location != loc) {
                continue;
            }

            if (trow == null || trow.getChildCount() == maxRowItems) {
                trow = new TableRow(context);
                trow.setLayoutParams(new LayoutParams(
                        LayoutParams.WRAP_CONTENT,
                        LayoutParams.WRAP_CONTENT
                ));
                table.addView(trow);
            }

            Button btn = (Button)context.getLayoutInflater().inflate
                (R.layout.keypopupbutton, null);
            btn.getBackground().setAlpha(popupBackAlpha);
            btn.setTextColor(popupButtonColor);            
            //Button btn = new Button(context);
            btn.setText(cmd.btn.getText());
            btn.setTag(new Integer(idx));
            btn.setOnClickListener(clickListener);            
            btn.setTypeface(cmd.btn.getTypeface());
            trow.addView(btn);
        }

        if (trow != null && trow.getChildCount() == 0) {
            table.removeView(trow);
        }

        if (table.getChildCount() == 0) return;

        int gravity = Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL;
        int y = 0;
        win.showAtLocation(parentView, gravity, 0, y);
    }

    public void toggleAutoListOpacity()
    {
        if (autoListWin == null || autoListTable == null) return; 

        autoListSolid = !autoListSolid;

        Log.d("Angband", "Auto List Opacity");

        int i, j;

        for (i = 0; i < autoListTable.getChildCount(); i++) {

            TableRow row = (TableRow)autoListTable.getChildAt(i);

            for (j = 0; j < row.getChildCount(); j++) {

                Button btn = (Button)row.getChildAt(j);

                int color = Color.WHITE;
                int alphaFg = 30;
                int alphaBg = 30;

                if (autoListSolid) {
                    alphaFg = 255;
                    alphaBg = 255;                    
                    color = 0x000044;
                }
                
                color = ColorUtils.setAlphaComponent(color, alphaFg);
                btn.setTextColor(color);                
                btn.getBackground().setAlpha(alphaBg);
            }
        }
    }

    public void showAutoList(View parentView) {

        removeAutoList();

        CmdLocation loc = CmdLocation.Dynamic; 

        int minRowItems = 3;
        float screenPct = 0.5f;
        if (context.landscapeNow()) {            
            screenPct = 0.65f;
        }

        Point winSize = context.getWinSize();

        int kbdH = context.getKeyboardHeight();
        int maxH = winSize.y - kbdH;
        int btnSize = kbdH / 2;
        int winH = 0;

        ArrayList<Command> the_list = new ArrayList<>();
        int i = 0;
        int j = 0;
        int[] sourceIdx = new int[200];
        for (Command cmd: commands) {
            if (cmd.location == loc && j < sourceIdx.length) {
                the_list.add(cmd);
                sourceIdx[j++] = i;
            }
            i++;
        }

        if (the_list.isEmpty()) return;   

        // Add the fixed keys at the left
        int listW = btnSize * (the_list.size() + 5);        
        // There is plenty of space
        if (listW < winSize.x) {
            return;
        }

        // Check if we can add more columns
        int maxRowItems = minRowItems;
        while ((maxRowItems * btnSize) < (int)(winSize.x * screenPct)) {
            ++maxRowItems;
        }
        maxRowItems = Math.max(maxRowItems-1, minRowItems);

        // Dummy entry, just to have another one
        the_list.add(the_list.get(0));

        int n = the_list.size();

        int lastIdx = n - 1;

        int rows = (n / maxRowItems);
        if (rows * maxRowItems < n) ++rows;

        winH = rows * btnSize;

        if (winH > 0 && winH < maxH) {
            winH = LayoutParams.WRAP_CONTENT;
        }
        else {
            winH = maxH;
        }

        final PopupWindow win = new PopupWindow(context);
                
        win.setOutsideTouchable(false);
        win.setTouchModal(false);
        win.getBackground().setAlpha(0);        

        win.setWidth(LayoutParams.WRAP_CONTENT);
        win.setHeight(winH);        

        ScrollView scroll = new ScrollView(context);
        scroll.setLayoutParams(new LayoutParams(
            LayoutParams.WRAP_CONTENT,
            LayoutParams.MATCH_PARENT
        ));        

        win.setContentView(scroll);

        TableLayout table = new TableLayout(context);        
        table.setLayoutParams(new LayoutParams(
            LayoutParams.WRAP_CONTENT,
            LayoutParams.MATCH_PARENT));
        scroll.addView(table);                

        TableRow trow = null;

        OnClickListener clickListener =
            new OnClickListener() {
                @Override
                public void onClick(View v) {
                    Integer tag = (Integer)v.getTag();
                    int savedIdx = tag.intValue();

                    if (savedIdx == -1 || !autoListSolid) {
                    //if (savedIdx == -1) {
                        toggleAutoListOpacity();
                        return;
                    }

                    // Indirection
                    Command cmd = commands.get(savedIdx);
                    if (cmd != null) cmd.btn.performClick();
                }
            };

        int idx = -1;        

        for (Command cmd: the_list) {
            ++idx;

            if (trow == null || trow.getChildCount() == maxRowItems) {
                trow = new TableRow(context);                
                trow.setLayoutParams(new LayoutParams(
                    LayoutParams.WRAP_CONTENT,
                    LayoutParams.WRAP_CONTENT));                
                trow.setGravity(Gravity.CENTER_VERTICAL);
                table.addView(trow);
            }

            Button btn = (Button)context.getLayoutInflater().inflate
                (R.layout.aribbonbutton, null);

            int alphaFg = 30;
            int alphaBg = 30;
            int color = Color.WHITE;
            
            //Button btn = new Button(context);
            if (idx == lastIdx) {
                alphaFg = 120;
                alphaBg = 60;
                if (fontCmd != null) {
                    btn.setTypeface(fontCmd);
                }                
                btn.setText("l");
                btn.setTag(new Integer(-1));
            }
            else {
                btn.setText(cmd.btn.getText());
                int realIdx = sourceIdx[idx];
                btn.setTag(new Integer(realIdx));
                btn.setTypeface(cmd.btn.getTypeface());
            }
            btn.setOnClickListener(clickListener);
            btn.setGravity(Gravity.CENTER);

            color = ColorUtils.setAlphaComponent(color, alphaFg);            
            btn.setTextColor(color);            
            btn.getBackground().setAlpha(alphaBg);

            resizeButton(btn);

            trow.addView(btn);
        }

        if (trow != null && trow.getChildCount() == 0) {
            table.removeView(trow);
        }

        if (table.getChildCount() == 0) {            
            return;
        }

        int gravity = Gravity.LEFT | Gravity.TOP;
        int y = 0;

        if (winH == LayoutParams.WRAP_CONTENT && kbdH > 0) {
            gravity = Gravity.LEFT | Gravity.BOTTOM;
            y = kbdH;
        }         

        autoListWin = win;
        autoListTable = table;

        //toggleAutoListOpacity();

        win.showAtLocation(parentView, gravity, 0, y);
    }

    public List<Integer> parseCodeKeys(String txt)
    {
        int i = 0;
        int n = txt.length();
        int ch0;
        char next;
        ArrayList<Integer> lst = new ArrayList<>();

        while (i < n) {
            ch0 = txt.charAt(i);
            next = 0;

            // We have another char
            if ((i+1) < n) next = txt.charAt(i+1);
                            
            if (ch0 == '^' && Character.isAlphabetic(next)) {
                ch0 = KTRL(next);
                i += 1;
            }
            else if (ch0 == '\\' &&
                Character.toLowerCase(next) == 'n') {
                ch0 = state.getKeyEnter();
                i += 1;
            }
            else if (ch0 == '\\' &&
                Character.toLowerCase(next) == 't') {
                ch0 = state.getKeyTab();
                i += 1;
            }
            else if (ch0 == '\\' &&
                Character.toLowerCase(next) == 's') {
                ch0 = ' ';
                i += 1;
            }
            else if (ch0 == '\\' &&
                Character.toLowerCase(next) == 'e') {
                ch0 = state.getKeyEsc();
                i += 1;
            }
            else if (ch0 == '\\') {
                // ignore, and next char too
                ch0 = 0;
                i += 1;
            }            

            if (ch0 > 0) lst.add(ch0);
            i += 1;
        }
        return lst;
    }

    public void procUserCommand(Command cmd)
    {
        String action = cmd.action.replace("USER_", "");
        Iterator<Integer> iter = parseCodeKeys(action).iterator();        
        while (iter.hasNext()) {
            Integer num = iter.next();
            state.addKey(num.intValue());            
        }        
    }

    @Override
    public void onClick(View v) {
        Command cmd = findCommand((Button)v);
        if (cmd == null) {
            return;
        }
        String action = cmd.action;

        if (cmd.isKeymap()) {
            state.addKey(cmd.getTrigger());
            return;
        }

        if (cmd.isUserCommand()) {
            procUserCommand(cmd);
            return;
        }

        if (cmd.isCommand()) {
            action = Character.toString(cmd.getCommand(true));
            //Log.d("Angband", "Command: " +
            //        printableChar(action.charAt(0)));
        }

        if (action.equals("Kbd")) {
            //showMenu();
            context.handler.sendEmptyMessage(AngbandDialog.Action.
                    OpenContextMenu.ordinal());
        }
        else if (action.equals("do_cmd_list")) {
            showCommandList(v);
        }
        else if (action.equals("Esc")) {
            state.addKey(state.getKeyEsc());
        }
        else if (action.equals("Ret")) {
            state.addKey(state.getKeyEnter());
        }
        else if (action.equals("Sft")) {
            setShift(!shifted);
        }
        else if (action.equals("Ctrl")) {
            setControlDown(!controlDown);
        }
        else if (action.equals("BackSpace")) {
            state.addKey(KC_BACKSPACE);
        }
        else if (action.equals("show_keys")) {
            showDynamicKeys(v);
        }
        else if (action.equals("tab")) {
            state.addKey(KC_TAB);
            setShift(false);
        }
        else if (isKtrl(action)) {
            int key = KTRL(action.charAt(1));
            state.addKey(key);
            setShift(false);
        }
        else if (action.length() == 1) {
            state.addKey(action.charAt(0));
            setShift(false);
        }
    }
}
