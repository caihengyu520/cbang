/******************************************************************************\

          This file is part of the C! library.  A.K.A the cbang library.

              Copyright (c) 2003-2017, Cauldron Development LLC
                 Copyright (c) 2003-2017, Stanford University
                             All rights reserved.

        The C! library is free software: you can redistribute it and/or
        modify it under the terms of the GNU Lesser General Public License
        as published by the Free Software Foundation, either version 2.1 of
        the License, or (at your option) any later version.

        The C! library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Lesser General Public License for more details.

        You should have received a copy of the GNU Lesser General Public
        License along with the C! library.  If not, see
        <http://www.gnu.org/licenses/>.

        In addition, BSD licensing may be granted on a case by case basis
        by written permission from at least one of the copyright holders.
        You may request written permission by emailing the authors.

                For information regarding this software email:
                               Joseph Coffland
                        joseph@cauldrondevelopment.com

\******************************************************************************/

#pragma once

#include <cbang/SmartPointer.h>
#include <cbang/iostream/Serializable.h>


namespace cb {
  namespace JSON {
    class Value;
    class Sink;

    class Serializable : public cb::Serializable {
    public:
      virtual void read(const Value &value) = 0;
      virtual void write(Sink &sink) const = 0;

      SmartPointer<Value> toJSON() const;

      // From cb::Serializable
      void read(std::istream &stream);
      void write(std::ostream &stream) const;
    };


    inline static
    Sink &operator<<(Sink &sink, const Serializable &s) {
      s.write(sink);
      return sink;
    }

    inline static
    const Value &operator>>(const Value &value, Serializable &s) {
      s.read(value);
      return value;
    }
  }
}
