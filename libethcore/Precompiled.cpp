/*
	This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file Precompiled.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#include "Precompiled.h"
#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcore/Hash.h>
#include <libdevcrypto/Common.h>
#include <libdevcrypto/LibSnark.h>
#include <libethcore/Common.h>
using namespace std;
using namespace dev;
using namespace dev::eth;

PrecompiledRegistrar* PrecompiledRegistrar::s_this = nullptr;

PrecompiledExecutor const& PrecompiledRegistrar::executor(std::string const& _name)
{
	if (!get()->m_execs.count(_name))
		BOOST_THROW_EXCEPTION(ExecutorNotFound());
	return get()->m_execs[_name];
}

namespace
{

ETH_REGISTER_PRECOMPILED(ecrecover)(bytesConstRef _in)
{
	struct
	{
		h256 hash;
		h256 v;
		h256 r;
		h256 s;
	} in;

	memcpy(&in, _in.data(), min(_in.size(), sizeof(in)));

	h256 ret;
	u256 v = (u256)in.v;
	if (v >= 27 && v <= 28)
	{
		SignatureStruct sig(in.r, in.s, (byte)((int)v - 27));
		if (sig.isValid())
		{
			try
			{
				if (Public rec = recover(sig, in.hash))
				{
					ret = dev::sha3(rec);
					memset(ret.data(), 0, 12);
					return ret.asBytes();
				}
			}
			catch (...) {}
		}
	}
	return {};
}

ETH_REGISTER_PRECOMPILED(sha256)(bytesConstRef _in)
{
	return dev::sha256(_in).asBytes();
}

ETH_REGISTER_PRECOMPILED(ripemd160)(bytesConstRef _in)
{
	return h256(dev::ripemd160(_in), h256::AlignRight).asBytes();
}

ETH_REGISTER_PRECOMPILED(identity)(bytesConstRef _in)
{
	return _in.toBytes();
}

ETH_REGISTER_PRECOMPILED(alt_bn128_pairing_product)(bytesConstRef _in, bytesRef _out)
{
	dev::snark::alt_bn128_pairing_product(_in, _out);
}

ETH_REGISTER_PRECOMPILED(alt_bn128_G1_add)(bytesConstRef _in, bytesRef _out)
{
	dev::snark::alt_bn128_G1_add(_in, _out);
}
ETH_REGISTER_PRECOMPILED(alt_bn128_G1_mul)(bytesConstRef _in, bytesRef _out)
{
	dev::snark::alt_bn128_G1_mul(_in, _out);
}


}
