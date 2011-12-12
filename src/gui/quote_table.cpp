#include "quote_table.hpp"
#include "cb/iostream.hpp"

using namespace cb;

QuoteTable::QuoteTable(int X, int Y, int W, int H, const char *L)
  : Fl_Table(X,Y,W,H,L), mQuoteList() {

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
  //col_width(0, 80);
  //col_width(1, 160);
  //col_width(2, 240);
  //col_width(3, 80);

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
  if (!ROW) ROW=1;  //compile warning prevention
  if (!mQuoteList.size()) return;
  fl_push_clip(X,Y,W,H);
    // Draw cell bg
    auto clr = FL_WHITE;
    /*switch (mpJobList->get(ROW)->getStatus()) {
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
    }*/
    fl_color(clr); fl_rectf(X,Y,W,H);
    // Draw cell data
    fl_color(FL_GRAY0); fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
    // Draw box border
    fl_color(color()); fl_rect(X,Y,W,H);
  fl_pop_clip();
} 

void QuoteTable::draw_cell(TableContext context, int ROW, int COL, int X, int Y, int W, int H) {
  if (ROW > int(mQuoteList.size())) return;
  switch (context) {
    case CONTEXT_STARTPAGE: {                 // before page is drawn..
        fl_font(FL_HELVETICA, 14);              // set the font for our drawing operations
        auto nrows = mQuoteList.size();
        nrows = nrows < MAX_ROWS ? nrows : MAX_ROWS;
        rows(nrows);
        break; 
    }
    case CONTEXT_ENDPAGE:
      break;
    case CONTEXT_ROW_HEADER:                  // Draw row headers
      //static char s[40];
      //sprintf(s,"%03d:",ROW);                 // "001:", "002:", etc
      drawHeader(mQuoteList[ROW].mSymbol.c_str(),X,Y,W,H);
      break; 
    case CONTEXT_COL_HEADER:                  // Draw column headers
      switch (QuoteCol(COL)) {
        case QuoteCol::LastPrice:
          drawHeader("Price", X, Y, W, H);
          break;
        case QuoteCol::LastSize:
          drawHeader("Size", X, Y, W, H);
          break;
        case QuoteCol::BidPrice:
          drawHeader("Bid", X, Y, W, H);
          break;
        case QuoteCol::BidSize:
          drawHeader("BidSz", X, Y, W, H);
          break;
        case QuoteCol::AskPrice:
          drawHeader("Ask", X, Y, W, H);
          break;
        case QuoteCol::AskSize:
          drawHeader("AskSz", X, Y, W, H);
          break;
        case QuoteCol::Volume:
          drawHeader("Vol", X, Y, W, H);
          break;
        default:
          drawHeader("?????", X, Y, W, H);
          break;
      }
      break; 
    case CONTEXT_CELL: {                      // Draw data in cells
      if (unsigned(ROW) < mQuoteList.size()) {
        stringstream ss;
        switch (QuoteCol(COL)) {
          case QuoteCol::LastPrice:
            ss << mQuoteList[ROW].mLastPrice;
            break;
          case QuoteCol::LastSize:
            ss << mQuoteList[ROW].mLastSize;
            break;
          case QuoteCol::BidPrice:
            ss << mQuoteList[ROW].mBidPrice;
            break;
          case QuoteCol::BidSize:
            ss << mQuoteList[ROW].mBidSize;
            break;
          case QuoteCol::AskPrice:
            ss << mQuoteList[ROW].mAskPrice;
            break;
          case QuoteCol::AskSize:
            ss << mQuoteList[ROW].mAskSize;
            break;
          case QuoteCol::Volume:
            ss << mQuoteList[ROW].mVolume;
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
      cout << "unknown context" << int(context) << endl;
      break;
  }
}

void QuoteTable::addSymbol(const string& symbol) {
  Qinfo qinfo;
  qinfo.mSymbol = symbol;
  qinfo.mLastPrice  = 0;
  qinfo.mLastSize   = 0;
  qinfo.mBidPrice   = 0;
  qinfo.mBidSize    = 0;
  qinfo.mAskPrice   = 0;
  qinfo.mAskSize    = 0;
  qinfo.mVolume    = 0;
  mQuoteList.push_back(qinfo);
  cout << "adding " << symbol << " to table.  size is now " << mQuoteList.size() << endl;
}

int QuoteTable::findIndex(const string& symbol) {
  for (auto it = mQuoteList.begin(); it != mQuoteList.end(); ++it) {
    if (it->mSymbol == symbol)
      return std::distance(mQuoteList.begin(), it);
  }
  return -1;
}

void QuoteTable::updateLastPrice(const string& symbol, const double price) {
  auto idx = findIndex(symbol);
  if (idx<0) return;
  mQuoteList[idx].mLastPrice = price;
}

void QuoteTable::updateLastSize(const string& symbol, const int size) {
  auto idx = findIndex(symbol);
  if (idx<0) return;
  mQuoteList[idx].mLastSize = size;
}

void QuoteTable::updateBidPrice(const string& symbol, const double price) {
  auto idx = findIndex(symbol);
  if (idx<0) return;
  mQuoteList[idx].mBidPrice = price;
}

void QuoteTable::updateBidSize(const string& symbol, const int size) {
  auto idx = findIndex(symbol);
  if (idx<0) return;
  mQuoteList[idx].mBidSize = size;
}

void QuoteTable::updateAskPrice(const string& symbol, const double price) {
  auto idx = findIndex(symbol);
  if (idx<0) return;
  mQuoteList[idx].mAskPrice = price;
}

void QuoteTable::updateAskSize(const string& symbol, const int size) {
  auto idx = findIndex(symbol);
  if (idx<0) return;
  mQuoteList[idx].mAskSize = size;
}

void QuoteTable::updateVolume(const string& symbol, const int size) {
  auto idx = findIndex(symbol);
  if (idx<0) return;
  mQuoteList[idx].mVolume = size;
}

