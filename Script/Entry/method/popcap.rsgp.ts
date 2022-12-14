/**
 * + popcap.rsgp.pack PopCap-RSGP 打包
 * + popcap.rsgp.unpack PopCap-RSGP 解包
 */
namespace TwinStar.Entry.method.popcap.rsgp {

	// ------------------------------------------------

	type Config = {
		pack_buffer_size: Executor.RequestArgument<string, false>;
	};

	export function _injector(
		config: Config,
	) {
		g_executor_method.push(
			Executor.method_of({
				id: 'popcap.rsgp.pack',
				descriptor(
				) {
					return Executor.query_method_description(this.id);
				},
				worker(a: Entry.CFSA & {
					bundle_directory: Executor.RequireArgument<string>;
					data_file: Executor.RequestArgument<string, true>;
					version_number: Executor.RequestArgument<bigint, false>;
					buffer_size: Executor.RequestArgument<string, false>;
				}) {
					let bundle_directory: string;
					let data_file: string;
					let version_number: [3n, 4n][number];
					let buffer_size: bigint;
					{
						bundle_directory = Executor.require_argument(
							...Executor.query_argument_message(this.id, 'bundle_directory'),
							a.bundle_directory,
							(value) => (value),
							(value) => (CoreX.FileSystem.exist_directory(value)),
						);
						data_file = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'data_file'),
							a.data_file,
							(value) => (value),
							() => (bundle_directory.replace(/((\.rsgp)(\.bundle))?$/i, '.rsgp')),
							...Executor.argument_requester_for_path('file', [false, a.fs_tactic_if_exist]),
						);
						version_number = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'version_number'),
							a.version_number,
							(value) => (value),
							null,
							() => (Console.option([0n, null, null, null, [3n, ''], [4n, '']], null)),
							(value) => ([3n, 4n].includes(value) ? null : localized(`版本不受支持`)),
						);
						buffer_size = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'buffer_size'),
							a.buffer_size,
							(value) => (parse_size_string(value)),
							null,
							() => (Console.size(null)),
							(value) => (null),
						);
					}
					let manifest_file = `${bundle_directory}/manifest.json`;
					let resource_directory = `${bundle_directory}/resource`;
					CoreX.Tool.PopCap.RSGP.pack_fs(data_file, manifest_file, resource_directory, { number: version_number }, buffer_size);
					Console.notify('s', localized(`执行成功`), [`${data_file}`]);
				},
				default_argument: {
					...Entry.k_cfsa,
					bundle_directory: undefined!,
					data_file: '?default',
					buffer_size: config.pack_buffer_size,
					version_number: '?input',
				},
				input_filter: Entry.file_system_path_test_generator([['directory', /.+(\.rsgp)(\.bundle)$/i]]),
				input_forwarder: 'bundle_directory',
			}),
			Executor.method_of({
				id: 'popcap.rsgp.unpack',
				descriptor(
				) {
					return Executor.query_method_description(this.id);
				},
				worker(a: Entry.CFSA & {
					data_file: Executor.RequireArgument<string>;
					bundle_directory: Executor.RequestArgument<string, true>;
					version_number: Executor.RequestArgument<bigint, false>;
				}) {
					let data_file: string;
					let bundle_directory: string;
					let version_number: [3n, 4n][number];
					{
						data_file = Executor.require_argument(
							...Executor.query_argument_message(this.id, 'data_file'),
							a.data_file,
							(value) => (value),
							(value) => (CoreX.FileSystem.exist_file(value)),
						);
						bundle_directory = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'bundle_directory'),
							a.bundle_directory,
							(value) => (value),
							() => (data_file.replace(/((\.rsgp))?$/i, '.rsgp.bundle')),
							...Executor.argument_requester_for_path('directory', [false, a.fs_tactic_if_exist]),
						);
						version_number = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'version_number'),
							a.version_number,
							(value) => (value),
							null,
							() => (Console.option([0n, null, null, null, [3n, ''], [4n, '']], null)),
							(value) => ([3n, 4n].includes(value) ? null : localized(`版本不受支持`)),
						);
					}
					let manifest_file = `${bundle_directory}/manifest.json`;
					let resource_directory = `${bundle_directory}/resource`;
					CoreX.Tool.PopCap.RSGP.unpack_fs(data_file, manifest_file, resource_directory, { number: version_number });
					Console.notify('s', localized(`执行成功`), [`${bundle_directory}`]);
				},
				default_argument: {
					...Entry.k_cfsa,
					data_file: undefined!,
					bundle_directory: '?default',
					version_number: '?input',
				},
				input_filter: Entry.file_system_path_test_generator([['file', /.+(\.rsgp)$/i]]),
				input_forwarder: 'data_file',
			}),
		);
	}

	// ------------------------------------------------

}

({
	injector: TwinStar.Entry.method.popcap.rsgp._injector,
});