// /**********
// © Copyright 2020 Xilinx, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// **********/

void datamover(input_window_cint16 * restrict cb_input, output_window_cint16 * restrict cb_output);
void datamover_scalar(input_window_cint16 * restrict cb_input, output_window_cint16 * restrict cb_output);
void datamover_vector_reg(input_window_cint16 * restrict cb_input, output_window_cint16 * restrict cb_output);
void datamover_mul_one(input_window_cint16 * restrict cb_input, output_window_cint16 * restrict cb_output);
void stream_datamover(input_stream_cint16 * restrict stream_in, output_stream_cint16 * restrict stream_out);
void datascale(input_window_cint16 * restrict cb_input, output_window_cint16 * restrict cb_output);

