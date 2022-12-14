/**
 * + data.hash.md5 MD5 散列
 */
namespace TwinStar.Entry.method.data.hash {

	// ------------------------------------------------

	type Config = {
	};

	export function _injector(
		config: Config,
	) {
		g_executor_method.push(
			Executor.method_of({
				id: 'data.hash.md5',
				descriptor(
				) {
					return Executor.query_method_description(this.id);
				},
				worker(a: Entry.CFSA & {
					file: Executor.RequireArgument<string>;
				}) {
					let file: string;
					{
						file = Executor.require_argument(
							...Executor.query_argument_message(this.id, 'file'),
							a.file,
							(value) => (value),
							(value) => (CoreX.FileSystem.exist_file(value)),
						);
					}
					let result = CoreX.Tool.Data.Hash.MD5.hash_fs(file);
					Console.notify('s', localized(`执行成功`), [`${result.toString(16)}`]);
				},
				default_argument: {
					...Entry.k_cfsa,
					file: undefined!,
				},
				input_filter: Entry.file_system_path_test_generator([['file', null]]),
				input_forwarder: 'file',
			}),
		);
	}

	// ------------------------------------------------

}

({
	injector: TwinStar.Entry.method.data.hash._injector,
});