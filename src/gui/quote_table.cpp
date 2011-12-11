#include "quote_table.hpp"
#include "core/job_list.hpp"
#include "cb/string.hpp"

using namespace cb;

QuoteTable::QuoteTable(int X, int Y, int W, int H, const char *L)
  : Fl_Table(X,Y,W,H,L), mpJobList(nullptr) {

  // Rows
  rows(MAX_ROWS);       // how many rows
  row_header(1);        // enable row headers (along left)
  row_height_all(20);   // default height of rows
  row_resize(0);        // disable row resizing

  // Cols
  cols(MAX_COLS);       // how many columns
  col_header(1);        // enable column headers (along top)
  col_width_all(80);    // default width of columns
  col_resize(0);        // disable column resizing

  // Static column widths
  col_width(0, 80);
  col_width(1, 160);
  col_width(2, 240);
  col_width(3, 80);

  end();                // end the Fl_Table group
}

QuoteTable::~QuoteTable() { 
}

void QuoteTable::drawHeader(const char *s, int X, int Y, int W, int H) {
  fl_push_clip(X,Y,W,H);
    fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, row_header_color());
    fl_color(FL_BLACK);
    fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
  fl_pop_clip();
} 

void QuoteTable::drawData(const char *s, int ROW, int COL, int X, int Y, int W, int H) {
  if (!COL) COL=1;  //compile warning prevention
  if (!mpJobList) return;
  fl_push_clip(X,Y,W,H);
    // Draw cell bg
    auto clr = FL_WHITE;
    switch (mpJobList->get(ROW)->getStatus()) {
      case DbStatus::DBERROR:
        clr = FL_RED;
        break;
      case DbStatus::SUCCESS:
        clr = FL_GREEN;
        break;
      case DbStatus::NONE:
        clr = FL_WHITE;
        break;
      default:
        clr = FL_YELLOW;
        break;
    }
    fl_color(clr); fl_rectf(X,Y,W,H);
    // Draw cell data
    fl_color(FL_GRAY0); fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
    // Draw box border
    fl_color(color()); fl_rect(X,Y,W,H);
  fl_pop_clip();
} 

void QuoteTable::draw_cell(TableContext context, int ROW, int COL, int X, int Y, int W, int H) {
  switch (context) {
    case CONTEXT_STARTPAGE:                   // before page is drawn..
      fl_font(FL_HELVETICA, 14);              // set the font for our drawing operations
      if (mpJobList) { 
        auto nrows = mpJobList->size();
        nrows = nrows < MAX_ROWS ? nrows : MAX_ROWS;
        rows(nrows);
      } else
        rows(0);
      break; 
    case CONTEXT_ENDPAGE:
      break;
    case CONTEXT_ROW_HEADER:                  // Draw row headers
      static char s[40];
      sprintf(s,"%03d:",ROW);                 // "001:", "002:", etc
      drawHeader(s,X,Y,W,H);
      break; 
    case CONTEXT_COL_HEADER:                  // Draw column headers
      switch (COL) {
        case 0:
          drawHeader("Date", X, Y, W, H);
          break;
        case 1:
          drawHeader("Query", X, Y, W, H);
          break;
        case 2:
          drawHeader("Parmameters", X, Y, W, H);
          break;
        case 3:
          drawHeader("Status", X, Y, W, H);
          break;
        default:
          drawHeader("?????", X, Y, W, H);
          break;
      }
      break; 
    case CONTEXT_CELL: {                      // Draw data in cells
      if (mpJobList && (unsigned)ROW < mpJobList->size()) {
        stringstream ss;
        switch (COL) {
          case 0:
            ss << "2011-12-05";
            break;
          case 1:
            ss << "Query Name";
            break;
          case 2:
            ss << "Paramaters 1,2,3,4,5,6,etc";
            break;
          case 3:
            ss << mpJobList->get(ROW)->getStatusString();
            break;
          default:
            ss << "Unknown";
            break;
        }
        drawData(ss.str().c_str(),ROW,COL,X,Y,W,H);
      }
      break;
    }
    case CONTEXT_RC_RESIZE: 
      break;
    default:
      cout << "unknown context" << context << endl;
      break;
  }
}

void QuoteTable::setJobList(JobList* jlist) {
  mpJobList = jlist;
}

