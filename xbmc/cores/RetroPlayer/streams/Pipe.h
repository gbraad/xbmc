/*
 *      Copyright (C) 2018 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "IDataStream.h"

namespace KODI
{
namespace STREAM
{
  class CPipe : public IDataStreamInput,
                public IDataStreamOutput
  {
  public:
    // Implementation of IDataStream
    bool IsOpen() override { return true; }
    void GetCapabilities(DataStreamCapabilities& caps) override;
    bool GetProperties(DataStreamProperties& props) override;
    bool GetDiagnostics(DataStreamDiagnostics& diags) override { return false; }

    // Implementation of IDataStream
    virtual bool Open() = 0;
    virtual bool Send(const uint8_t* data, size_t size) = 0;
    virtual void Close() = 0;

    // Implementation of IDataStreamInput
    virtual void OnOpen() = 0;
    virtual void OnReceive(const uint8_t* data, size_t size) = 0;
    virtual void OnClose(int error) = 0;
  };
}
}
