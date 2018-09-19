/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2009 Jean-Pierre Charras, jean-pierre.charras@ujf-grenoble.fr
 * Copyright (C) 1992-2016 KiCad Developers, see AUTHORS.txt for contributors.
 * Copyright (C) 2018 CERN
 * Author: Maciej Suminski <maciej.suminski@cern.ch>
 * Author: Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "pcbnew_printout.h"
#include <class_board.h>

#include <pcb_painter.h>

PCBNEW_PRINTOUT::PCBNEW_PRINTOUT( BOARD* aBoard, const PRINT_PARAMETERS& aParams,
        const KIGFX::VIEW* aView, const wxSize& aSheetSize, const wxString& aTitle ) :
    BOARD_PRINTOUT( aParams, aView, aSheetSize, aTitle )
{
    m_board = aBoard;
}


bool PCBNEW_PRINTOUT::OnPrintPage( int aPage )
{
    LSET lset = m_PrintParams.m_PrintMaskLayer;
    int pageCount = lset.count();
    wxString layer;
    PCB_LAYER_ID extractLayer;

    // compute layer mask from page number if we want one page per layer
    if( m_PrintParams.m_OptionPrintPage == 0 )  // One page per layer
    {
        // This sequence is TBD, call a different
        // sequencer if needed, such as Seq().  Could not find documentation on
        // page order.
        LSEQ seq = lset.UIOrder();

        // aPage starts at 1, not 0
        if( unsigned( aPage - 1 ) < seq.size() )
            m_PrintParams.m_PrintMaskLayer = LSET( seq[aPage - 1] );
    }

    if( !m_PrintParams.m_PrintMaskLayer.any() )
        return false;

    extractLayer = m_PrintParams.m_PrintMaskLayer.ExtractLayer();

    if( extractLayer == UNDEFINED_LAYER )
        layer = _( "Multiple Layers" );
    else
        layer = LSET::Name( extractLayer );

    // In Pcbnew we can want the layer EDGE always printed
    if( m_PrintParams.m_Flags == 1 )
        m_PrintParams.m_PrintMaskLayer.set( Edge_Cuts );

    DrawPage( layer, aPage, pageCount );

    m_PrintParams.m_PrintMaskLayer = lset;

    return true;
}


EDA_RECT PCBNEW_PRINTOUT::getBoundingBox()
{
    return m_board->ComputeBoundingBox();
}


std::unique_ptr<KIGFX::PAINTER> PCBNEW_PRINTOUT::getPainter( KIGFX::GAL* aGal )
{
    return std::unique_ptr<KIGFX::PAINTER>( new KIGFX::PCB_PAINTER( aGal ) );
}