//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Implementation of document class
//
// This file is part of Gambit
// Copyright (c) 2004, The Gambit Project
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include <fstream>
#include <sstream>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif  // WX_PRECOMP

#include "game-document.h"

static wxColour s_defaultColors[8] = {
  wxColour(255, 0, 0),
  wxColour(0, 0, 255),
  wxColour(0, 128, 0),
  wxColour(255, 128, 0),
  wxColour(0, 0, 64),
  wxColour(128, 0, 255),
  wxColour(64, 0, 0),
  wxColour(255, 128, 255)
};


gbtGameDocument::gbtGameDocument(const gbtGame &p_game)
  : m_game(p_game), m_modified(false), m_treeZoom(1.0)
{
  if (!m_game.IsNull()) {
    for (int pl = 1; pl <= m_game->NumPlayers(); pl++) {
      m_playerColors.Append(s_defaultColors[(pl - 1) % 8]);
    }
  }
}


void gbtGameDocument::AddView(gbtGameView *p_view)
{ 
  m_views.Append(p_view);
}

void gbtGameDocument::RemoveView(gbtGameView *p_view)
{ 
  m_views.Remove(m_views.Find(p_view)); 
  if (m_views.Length() == 0) delete this;
}

void gbtGameDocument::UpdateViews(void)
{
  for (int i = 1; i <= m_views.Length(); m_views[i++]->OnUpdate());
}

//-----------------------------------------------------------------------
//         gbtGameDocument: Operations modifying the document
//-----------------------------------------------------------------------

gbtGameOutcome gbtGameDocument::NewOutcome(void) 
{ 
  gbtGameOutcome r = m_game->NewOutcome(); 
  m_modified = true;
  UpdateViews(); 
  return r;
}

void gbtGameDocument::SetPayoff(gbtGameOutcome p_outcome,
				const gbtGamePlayer &p_player, 
				const gbtRational &p_value)
{ 
  p_outcome->SetPayoff(p_player, p_value); 
  m_modified = true;
  UpdateViews(); 
}


wxColour gbtGameDocument::GetPlayerColor(int p_player) const
{
  if (p_player == 0) {
    return *wxLIGHT_GREY;
  }
  else {
    return m_playerColors[p_player];
  }
}

void gbtGameDocument::SetPlayerColor(int p_player, const wxColour &p_color)
{
  m_playerColors[p_player] = p_color;
  UpdateViews();
}


void gbtGameDocument::Load(const wxString &p_filename)
{
  std::ifstream file(p_filename.c_str());

  while (!file.eof()) {
    std::string key, value;
    char c;
    
    while (!file.eof() && (c = file.get()) != '=') {
      key += c;
    }

    while (!file.eof() && (c = file.get()) != '\n') {
      value += c;
    }

    if (key == "efg") {
      std::istringstream iss(value);
      m_game = ReadEfg(iss);
    }
    else if (key == "nfg") {
      std::istringstream iss(value);
      m_game = ReadNfg(iss);
    }
    else if (key == "playercolor") {
      int pl, r, g, b;
      sscanf(value.c_str(), "%d %d %d %d", &pl, &r, &g, &b);
      if (m_playerColors.Last() >= pl) {
	m_playerColors[pl] = wxColour(r, g, b);
      }
      else {
	m_playerColors.Append(wxColour(r, g, b));
      }
    }
  }
}

void gbtGameDocument::Save(const wxString &p_filename) const
{
  std::ostringstream oss;
  if (m_game->HasTree()) {
    m_game->WriteEfg(oss);
  }
  else {
    m_game->WriteNfg(oss);
  }
  std::string gamefile = oss.str();
  for (int i = 0; i < gamefile.length(); i++) {
    if (gamefile[i] == '\n')  gamefile[i] = ' ';
  }

  std::ofstream file(p_filename.c_str());
  if (m_game->HasTree()) {
    file << "efg= " << gamefile << std::endl;
  }
  else {
    file << "nfg= " << gamefile << std::endl;
  }
  for (int pl = 1; pl <= m_game->NumPlayers(); pl++) {
    file << "playercolor= " << pl << " ";
    file << (int) m_playerColors[pl].Red() << " ";
    file << (int) m_playerColors[pl].Green() << " ";
    file << (int) m_playerColors[pl].Blue() << std::endl;
  }
}



gbtGameView::gbtGameView(gbtGameDocument *p_doc)
  : m_doc(p_doc)
{ m_doc->AddView(this); }

gbtGameView::~gbtGameView()
{ m_doc->RemoveView(this); }
