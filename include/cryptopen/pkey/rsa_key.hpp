/*
 * libcryptopen - C++ portable OpenSSL cryptographic wrapper library.
 * Copyright (C) 2010-2011 Julien Kauffmann <julien.kauffmann@freelan.org>
 *
 * This file is part of libcryptopen.
 *
 * libcryptopen is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * libcryptopen is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 *
 * If you intend to use libcryptopen in a commercial software, please
 * contact me : we may arrange this for a small fee or no fee at all,
 * depending on the nature of your project.
 */

/**
 * \file rsa_key.hpp
 * \author Julien KAUFFMANN <julien.kauffmann@freelan.org>
 * \brief A RSA key class.
 */

#ifndef CRYPTOPEN_PKEY_RSA_KEY_HPP
#define CRYPTOPEN_PKEY_RSA_KEY_HPP

#include "../error/cryptographic_exception.hpp"

#include <openssl/rsa.h>
#include <openssl/engine.h>

#include <boost/shared_ptr.hpp>

namespace cryptopen
{
	namespace pkey
	{
		/**
		 * \brief A RSA key.
		 *
		 * The rsa_key class represents a RSA key (with or without a private compound).
		 * rsa_key is a low level structure that offers no mean to know whether the represented RSA key is a public or private key: it is up to the user to ensure that private key related functions will only get called on rsa_key instances that have private key information.
		 *
		 * A rsa_key instance has the same semantic as a RSA* pointer, thus two copies of the same instance share the same underlying RSA* pointer.
		 */
		class rsa_key
		{
			public:

				/**
				 * \brief A generate callback type.
				 */
				typedef void (*generate_callback_type)(int, int, void*);

				/**
				 * \brief Generate a new RSA key.
				 * \param num The size (in bits) of the modulus. As specified in OpenSSL documentation, key sizes with num < 1024 should be considered insecure.
				 * \param exponent The exponent. Must be an odd number: typically 3, 17 or 65537.
				 * \param callback A callback that will get notified about the key generation, as specified in the documentation of RSA_generate_key(3). callback might be NULL (the default).
				 * \param callback_arg An argument that will be passed to callback, if needed.
				 */
				static rsa_key generate(int num, unsigned long exponent, generate_callback_type callback = NULL, void* callback_arg = NULL);

				/**
				 * \brief Create a new empty RSA key.
				 *
				 * If allocation fails, a cryptographic_exception is thrown.
				 */
				rsa_key();

				/**
				 * \brief Enable blinding of the rsa_key to prevent timing attacks.
				 * \param ctx A BN_CTX to use or NULL (the default) if one is to be created.
				 * \warning The PRNG must be seeded prior to calling enable_blinding().
				 * \see disable_blinding
				 */
				void enable_blinding(BN_CTX* ctx = NULL);

				/**
				 * \brief Disable blinding on the rsa_key after a previous call to enable_blinding().
				 * \see enable_blinding
				 */
				void disable_blinding();

				/**
				 * \brief Get the raw RSA pointer.
				 * \return The raw RSA pointer.
				 * \warning The instance has ownership of the return pointer. Calling RSA_free() on the returned value will result in undefined behavior.
				 */
				RSA* raw();

				/**
				 * \brief Get the raw RSA pointer.
				 * \return The raw RSA pointer.
				 * \warning The instance has ownership of the return pointer. Calling RSA_free() on the returned value will result in undefined behavior.
				 */
				const RSA* raw() const;

			private:

				explicit rsa_key(boost::shared_ptr<RSA> rsa);

				// Here a boost::unique_ptr would have much less overhead, but this requires C++1x
				boost::shared_ptr<RSA> m_rsa;
		};

		/**
		 * \brief Compare two rsa_key instances.
		 * \param lhs The left argument.
		 * \param rhs The right argument.
		 * \return true if the two rsa_key instance share the same underlying RSA* pointer.
		 */
		bool operator==(const rsa_key& lhs, const rsa_key& rhs);

		/**
		 * \brief Compare two rsa_key instances.
		 * \param lhs The left argument.
		 * \param rhs The right argument.
		 * \return true if the two rsa_key instance do not share the same underlying RSA* pointer.
		 */
		bool operator!=(const rsa_key& lhs, const rsa_key& rhs);
		
		inline rsa_key::rsa_key() : m_rsa(RSA_new(), RSA_free)
		{
			error::throw_error_if_not(m_rsa);
		}
		inline void rsa_key::enable_blinding(BN_CTX* ctx)
		{
			error::throw_error_if_not(RSA_blinding_on(m_rsa.get(), ctx));
		}
		inline void rsa_key::disable_blinding()
		{
			RSA_blinding_off(m_rsa.get());
		}
		inline RSA* rsa_key::raw()
		{
			return m_rsa.get();
		}
		inline const RSA* rsa_key::raw() const
		{
			return m_rsa.get();
		}
		inline rsa_key::rsa_key(boost::shared_ptr<RSA> rsa) : m_rsa(rsa)
		{
			error::throw_error_if_not(m_rsa);
		}
		inline bool operator==(const rsa_key& lhs, const rsa_key& rhs)
		{
			return lhs.raw() == rhs.raw();
		}
		inline bool operator!=(const rsa_key& lhs, const rsa_key& rhs)
		{
			return lhs.raw() != rhs.raw();
		}
	}
}

#endif /* CRYPTOPEN_PKEY_RSA_KEY_HPP */
