package org.rephial.xyangband;

import android.util.Log;

import java.util.HashMap;
import java.util.Map;

public class TermWindow {


	public static class ColorPair {
		public int fColor;
		public int bColor;
		public ColorPair(int f,int b) {this.fColor = f; this.bColor = b;}
	}
	public static Map<Integer,ColorPair> pairs = new HashMap<Integer,ColorPair>(); 
	public static Map<Integer,Integer> color_table = new HashMap<Integer, Integer>(); 
	public static int TERM_BLACK = 0xFF000000;
	public static int TERM_WHITE = 0xFFFFFFFF;
	public static ColorPair defaultColor = new ColorPair(TERM_WHITE,TERM_BLACK);

	public class TermPoint {
		public char ch = ' ';
		public int bgColor = 0;
		public int fgColor = 0;
		public boolean isDirty = false;
		public boolean isUgly = false;

		public void clear() {
			isDirty = isDirty || ch != ' ' || fgColor != 0 || bgColor != 0;
			ch = ' ';
			bgColor = 0;
			fgColor = 0;
		}
	}
	public TermPoint[][] buffer = null; 

	public boolean allDirty = false;
	public boolean cursor_visible;
	public int col = 0;
	public int row = 0;
	public int cur_color = 0;
	public int cur_bg_color = 0;

	public int cols = 0;
	public int rows = 0;
	public int begin_y = 0;
	public int begin_x = 0;

	public TermWindow(int rows, int cols, int begin_y, int begin_x) {
		if (cols == 0) this.cols = Preferences.getTermWidth();
		else this.cols = cols;
		if (rows == 0) this.rows = Preferences.getTermHeight();
		else this.rows = rows;
		this.begin_y = begin_y;
		this.begin_x = begin_x;
		buffer = new TermPoint[this.rows][this.cols];
		for (int r=0;r<this.rows;r++) {
			for (int c=0;c<this.cols;c++) {
				buffer[r][c] = new TermPoint();
			}
		}
	}

	public void resize(int width, int height)
	{
		this.cols = width;
		this.rows = height;
		// Overflow
		if (this.col >= this.cols) this.col = 0;
		if (this.row >= this.rows) this.row = 0;
		this.buffer = new TermPoint[this.rows][this.cols];
		for (int r=0;r<this.rows;r++) {
			for (int c=0;c<this.cols;c++) {
				buffer[r][c] = new TermPoint();
			}
		}
	}

	public static void init_color(int c, int rgb) {
		color_table.put(c, rgb);
	}

	public static void init_pair(int p, int f, int b) {
		int fc = color_table.get(f);
		int bc = color_table.get(b);

		pairs.put(p, new ColorPair(fc,bc));
	}

	public void clearPoint(int row, int col) {
		if (col >= 0 && col < cols &&
				row >= 0 && row < rows) {
			buffer[row][col].clear();
		} else {
			Log.d("Angband","TermWindow.clearPoint - point out of bounds: "+col+","+row);
		}
	}

	public TermPoint getPoint(int dy, int dx)
	{
		if (this.row + this.col == 0) {
			return null;
		}

		int y = this.row + dy;
		int x = this.col + dx;

		if (y < this.begin_y || y >= this.rows ||
				x < this.begin_x || x >= this.cols ||
				this.buffer == null) {
			return null;
		}
		return this.buffer[y][x];
	}

	protected void attrset(int a) {
		cur_color = a;
	}

	protected void bgattrset(int a) {
		cur_bg_color = a;
	}

	public void clear() {
		//Log.d("Angband","TermWindow.clear start "+rows+","+cols);
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < cols; c++) {
				//Log.d("Angband","TermWindow.clear clearPoint "+r+","+c);
				buffer[r][c].clear();
			}
		}
		//Log.d("Angband","TermWindow.clear end");
	}

	public void clrtoeol() {
		//Log.d("Angband","TermWindow.clrtoeol ("+row+","+col+")");
		for (int c=col;c<cols;c++) {
			buffer[row][c].clear();
		}
	}

	public void clrtobot() {
		//Log.d("Angband","TermWindow.clrtobot ("+row+","+col+")");
		for (int r=row;r<rows;r++) {
			for (int c=col;c<cols;c++) {
				buffer[r][c].clear();
			}
		}
	}

	public void hline(char c, int n) {
		//Log.d("Angband","TermWindow.hline ("+row+","+col+") "+n);
		int x = Math.min(cols,n+col);
		for (int i=col;i<x;i++) {
			addch(c);
		}
	}

	public void move(int row, int col) {
		if (col >= 0 && col < cols && row >= 0 & row<rows) {
			this.col = col;
			this.row = row;
		}
	}

	public int inch() {
		return buffer[this.row][this.col].ch;
	}

	public int mvinch(int row, int col) {
		move(row,col);
		return inch();
	}

	public int attrget(int row, int col) {
		if (col>-1 && col<cols && row>-1 & row<rows) {
			return buffer[row][col].fgColor;
		}
		return -1;
	}

	public int getcury() {
		return row;
	}

	public int getcurx() {
		return col;
	}

	public void overwrite(TermWindow wsrc) {

		int sx0 = wsrc.begin_x;
		int sx1 = wsrc.begin_x+wsrc.cols-1;
		int sy0 = wsrc.begin_y;
		int sy1 = wsrc.begin_y+wsrc.rows-1;
		int dx0 = begin_x;
		int dx1 = begin_x+cols-1;
		int dy0 = begin_y;
		int dy1 = begin_y+rows-1;

		// do wins intersect?
		if (!(sx0 > dx1 || sx1 < dx0 || sy0 > dy1 || sy1 < dy0)) {

			// calc intersect area
			int ix0 = Math.max(sx0,dx0);
			int iy0 = Math.max(sy0,dy0);
			int ix1 = Math.min(sx1,dx1);
			int iy1 = Math.min(sy1,dy1);

			// blit the ascii
			for (int r=iy0;r<=iy1;r++) {
				for (int c=ix0;c<=ix1;c++) {
					TermPoint p1 = wsrc.buffer[r-wsrc.begin_y][c-wsrc.begin_x];
					TermPoint p2 = buffer[r-begin_y][c-begin_x];
					if (p2.ch != p1.ch ||
							p2.fgColor != p1.fgColor ||
							p2.bgColor != p2.bgColor) {
						p2.isDirty = true;
						p2.ch = p1.ch;
						p2.bgColor = p1.bgColor;
						p2.fgColor = p1.fgColor;
					}
				}
			}
		}
	}

	public void touch() {
		for (int r=0;r<rows;r++) {
			for (int c=0;c<cols;c++) {
				TermPoint p = buffer[r][c];
				p.isDirty = true;
			}
		}
	}

	public void addnstr(int n, byte[] cp) {
		/*
		String foo = new String(cp);
		Log.d("Angband","addnstr ("+row+","+col+") ["+foo+"]");
		*/

		String str;
		try {
			//str = new String(cp, "ISO-8859-1");
			str = new String(cp, "UTF-8");
		} catch(java.io.UnsupportedEncodingException e) {
			str = "";
		}

		// Overwrite n, because the parameter was expressed in bytes,
		// not in UTF-8 characters
		n = str.length();
		
		for (int i = 0; i < n; i++) {
		    addch(str.charAt(i));
		}
	}

	public void addch(char c) {
		
		/*
		Formatter fmt = new Formatter();
		fmt.format("color: %x", cur_color);
		Log.d("Angband","TermWindow.addch ("+row+","+col+") "+fmt+" '"+c+"'");
		*/

		if (col>-1 && col<cols && row>-1 && row<rows) {
			if (c > 19) {
				TermPoint p = buffer[row][col];
			
				if (p.ch != c ||
						p.fgColor != cur_color ||
						p.bgColor != cur_bg_color) {
					p.isDirty = true;
					p.ch = c;
					p.fgColor = cur_color;
					p.bgColor = cur_bg_color;
				}

				advance();
			}
			else if (c == 9) {  // recurse to expand that tab
				//Log.d("Angband","TermWindow.addch - tab expand");				
				int ss = col % 8;
				if (ss==0) ss=8;
				for (int i=0;i<ss;i++) addch(' ');
			}
			else {
				//Log.d("Angband","TermWindow.addch - invalid character: "+(int)c);
				advance();
			}
		}
		else {
			Log.d("Angband","TermWindow.addch - point out of bounds: "+col+","+row);
		}	
	}

	private void advance() {
		col++;
		if (col >= cols) {
			row++;
			col = 0;
		}
		if (row >= rows) {
			row = rows-1;
		}
	}

	public void mvaddch(int row, int col, char c) {
		move(row, col);
		addch(c);
	}

	public void scroll() {
		for (int r=1;r<rows;r++) {
			for (int c=0;c<cols;c++) {
				buffer[r-1][c] = buffer[r][c];
			}
		}
	}
}