/*
* Copyright (c) 2021-2022 NVIDIA Corporation
 *
 * Licensed under the Apache License Version 2.0 with LLVM Exceptions
 * (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *   https://llvm.org/LICENSE.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Pull in the reference implementation of P2300:
#include <stdexec/execution.hpp>

using namespace stdexec;

int f() {
	return 239;
}

struct fun {
	struct impl_t {
		template<class _Cvref, class _Fun>
		auto operator() (_Cvref, _Fun && __fun)
		{
			return __fun(just, __tuple<std::index_sequence<>*>{});
		}
	};
	auto operator()(auto sched) const {
		impl_t impl;
		__sexpr<impl_t> just_{impl};
		auto sndr = then(just_, f);
		return on(sched, sndr);
	}
};

using Sender = __sexpr<__detail::_Lambda<
	on_t,
	run_loop::__scheduler,
	__sexpr<__detail::_Lambda<then_t, int(*)(), __sexpr<fun::impl_t>>>
>>;

void test(Sender s) {
	using completion_signatures_t = decltype(get_completion_signatures(s, __sync_wait::__env{}));

	using _Fun = __detail::__always<__sexpr<__detail::_Lambda<then_t, int(*)(), __sexpr<fun::impl_t>>>>;
	using _Domain = __let::__result_domain_t<set_value_t, run_loop::__scheduler::__schedule_task, _Fun, __sync_wait::__env, run_loop::__scheduler>;
	static_assert(__ok<_Domain>);
}
