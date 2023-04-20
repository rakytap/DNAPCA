#include <enums.h>
#include <maxtx_config.h>
#include <maxtx_runtime.h>

int test() {
    int num_nodes = 1;
    int num_tsps_per_node = 1;
    int inner_dimension = 7680;
    int num_eigenvectors = 3;
    int batch_size = 32000;
    int num_batches = 3;
    int max_batches = 1;
    const char *xtx_iop_dir = "../iops/xtx";
    const char *matmul_iop_dir = "../iops/matmul";
    const char *build_dir = ".";
    const char *input_data_dir = "./xtx_input_7680_96000_fp32";
    const char *output_data_dir = "./output";



    Config config = initialise_config(build_dir);

    Node node = create_node(config);

    Buffer input = create_node_buffer(max_batches, F32, BATCHED, node);
    Buffer xtx_result = create_node_buffer(max_batches, F32, FIXED, node);
    Buffer eigenvector_result =
        create_node_buffer(max_batches, F32, FIXED, node);
    Buffer matmul_input = create_node_buffer(max_batches, F32, BATCHED, node);
    Buffer transpose_input =
        create_node_buffer(max_batches, F32, BATCHED, node);
    Buffer output = create_node_buffer(max_batches, F32, BATCHED, node);

    create_file_input(inner_dimension, F32, input, node, config);
    create_file_input(inner_dimension, F32, matmul_input, node, config);
    create_file_output(output, node, config);

    calculate_correlation_tsp(inner_dimension, num_nodes, num_tsps_per_node,
                              &input, &xtx_result, F32, xtx_iop_dir, &node,
                              config);
    eigenvectors_cpu(num_eigenvectors, xtx_result, eigenvector_result, node,
                     config);

    transpose_cpu(matmul_input, transpose_input, node, config);
    multiply_batched_matrix_fixed_vector_tsp(
        inner_dimension, num_eigenvectors, num_tsps_per_node, matmul_iop_dir,
        transpose_input, eigenvector_result, output, node, config);

    finalise_config(config);

    Calculation calc = load_configuration(build_dir);

    set_input_location(input_data_dir, 0, calc);
    set_input_batch_size(batch_size, num_batches, 0, calc);
    set_input_location(input_data_dir, 1, calc);
    set_input_batch_size(batch_size, num_batches, 1, calc);
    set_output_location(output_data_dir, 0, calc);

    deploy_calculation(calc);

    run_calculation(calc, 1);

    return 0;
}
