#ifndef QUOTE_TABLE_HPP
#define QUOTE_TABLE_HPP

#include <vector>
#include "cb/string.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>

#define MAX_ROWS 100
#define MAX_COLS 8

namespace cb {

struct Qinfo {
  string      mSymbol;
  double      mLastPrice;
  int         mLastSize;
  double      mBidPrice;
  int         mBidSize;
  double      mAskPrice;
  int         mAskSize;
  int         mVolume;
};

enum class QuoteCol {LastPrice, LastSize, BidPrice, BidSize, AskPrice, AskSize, Volume};

class QuoteTable : public Fl_Table {
public:
  QuoteTable(int X, int Y, int W, int H, const char *L=0);
  ~QuoteTable(); 

  void draw_cell(TableContext context, int ROW=0, int COL=0, int X=0, int Y=0, int W=0, int H=0);

  void addSymbol(const string& symbol);
  int findIndex(const string& symbol);
  void updateLastPrice(const string& symbol, const double price);
  void updateLastSize(const string& symbol, const int size);
  void updateBidPrice(const string& symbol, const double price);
  void updateBidSize(const string& symbol, const int size);
  void updateAskPrice(const string& symbol, const double price);
  void updateAskSize(const string& symbol, const int size);
  void updateVolume(const string& symbol, const int size);

private:
  void drawHeader(const char *s, int X, int Y, int W, int H);
  void drawData(const char *s, int ROW, int COL, int X, int Y, int W, int H);

  std::vector<Qinfo>  mQuoteList;
};

}
#endif //QUOTE_TABLE_HPP

