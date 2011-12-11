#ifndef QUOTE_TABLE_HPP
#define QUOTE_TABLE_HPP

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>

#define MAX_ROWS 100
#define MAX_COLS 4

namespace cb {

class JobList;

class QuoteTable : public Fl_Table {
public:
  QuoteTable(int X, int Y, int W, int H, const char *L=0);
  ~QuoteTable(); 

  void setJobList(JobList* jlist);
  void draw_cell(TableContext context, int ROW=0, int COL=0, int X=0, int Y=0, int W=0, int H=0);

private:
  void drawHeader(const char *s, int X, int Y, int W, int H);
  void drawData(const char *s, int ROW, int COL, int X, int Y, int W, int H);

  JobList* mpJobList;
};

}
#endif //QUOTE_TABLE_HPP

