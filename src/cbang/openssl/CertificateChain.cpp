/******************************************************************************\

          This file is part of the C! library.  A.K.A the cbang library.

                Copyright (c) 2003-2019, Cauldron Development LLC
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

#include "CertificateChain.h"

#include "Certificate.h"
#include "BIStream.h"

#include <openssl/pem.h>
#include <openssl/x509_vfy.h>
#include <openssl/opensslv.h>

using namespace cb;
using namespace std;


#if OPENSSL_VERSION_NUMBER < 0x1010000fL
#define X509_up_ref(c) CRYPTO_add(&(c)->references, 1, CRYPTO_LOCK_EVP_PKEY)
#endif // OPENSSL_VERSION_NUMBER < 0x1010000fL

#if OPENSSL_VERSION_NUMBER < 0x1000200fL
X509_CHAIN *X509_chain_up_ref(X509_CHAIN *chain) {
  X509_CHAIN *copy = sk_X509_dup(chain);

  for (int i = 0; i < sk_X509_num(copy); i++)
    X509_up_ref(sk_X509_value(copy, i));

  return copy;
}
#endif // OPENSSL_VERSION_NUMBER < 0x1000200fL


CertificateChain::CertificateChain(const CertificateChain &o) :
  chain(X509_chain_up_ref(o.chain)) {}


CertificateChain::CertificateChain(X509_CHAIN *chain) : chain(chain) {
  if (!chain) this->chain = sk_X509_new_null();
}


CertificateChain::~CertificateChain() {clear();}


CertificateChain &CertificateChain::operator=(const CertificateChain &o) {
  sk_X509_pop_free(chain, X509_free);
  chain = X509_chain_up_ref(o.chain);
  return *this;
}


unsigned CertificateChain::size() const {return sk_X509_num(chain);}


Certificate CertificateChain::get(unsigned i) const {
  if (size() <= i) THROW("Invalid certificate chain index " << i);
  return Certificate(sk_X509_value(chain, i));
}


void CertificateChain::add(const Certificate &cert) {
  X509_up_ref(cert.getX509());
  sk_X509_push(chain, cert.getX509());
}


void CertificateChain::clear() {sk_X509_pop_free(chain, X509_free);}


void CertificateChain::read(istream &stream) {
  BIStream bio(stream);

  X509 *cert = 0;

  while (PEM_read_bio_X509(bio.getBIO(), &cert, 0, 0)) {
    add(Certificate(cert));
    cert = 0;
  }
}


void CertificateChain::write(ostream &stream) const {
  for (unsigned i = 0; i < size(); i++)
    get(i).write(stream);
}
