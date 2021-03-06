#include "caffe2/c10_prototype/c10.h"
#include <utility>
#include "caffe2/core/dispatch/OpSchemaRegistration.h"
#include "caffe2/core/dispatch/KernelRegistration.h"
#include "caffe2/c10_prototype/c10/cpu/CPUTensorImpl.h"
#include "caffe2/c10_prototype/c10/stack_bindings/StackBasedOperatorRegistry.h"
#include "caffe2/c10_prototype/c10/stack_bindings/ParameterStack.h"
#include <iostream>

using namespace c10;

namespace ops {
struct conditional final {
  using Signature = Tensor(bool, const Tensor&, Tensor);

  static constexpr guts::array<const char*, 3> parameter_names = {{
    "condition", "lhs", "rhs"
  }};
};
}
C10_DEFINE_OP_SCHEMA(::ops::conditional);

Tensor conditional_op(bool condition, const Tensor& thenTensor, Tensor elseTensor) {
  if (condition) {
    return thenTensor;
  } else {
    return elseTensor;
  }
}

// TODO Commented out because I removed c10::Tensor compatibility from OpSchema
//C10_REGISTER_KERNEL(::ops::conditional)
//  .kernel(&conditional_op)
//  .dispatchKey({c10::details::TensorParameterDispatchKey{DeviceId::CPU, LayoutId(0), caffe2::TypeMeta::Id<int>()}, c10::details::TensorParameterDispatchKey{DeviceId::CPU, LayoutId(0), caffe2::TypeMeta::Id<int>()}});

int main() {
  ParameterStack callStack;

  callStack.push(tensor<int>({5}));
  callStack.push(tensor<int>({10}));
  callStack.push(true);

  auto op = StackBasedOperatorRegistry()->Create("::ops::conditional");
  (*op)(&callStack);

  Tensor result = callStack.pop<Tensor>();
  
  std::cout << "Result is " << result.data<int>()[0] << std::endl;
}
