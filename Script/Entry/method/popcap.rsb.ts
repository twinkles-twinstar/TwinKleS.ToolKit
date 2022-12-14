/**
 * + popcap.rsb.pack PopCap-RSB 打包
 * + popcap.rsb.unpack PopCap-RSB 解包
 * + popcap.rsb.extract_resource PopCap-RSB 资源提取
 */
namespace TwinStar.Entry.method.popcap.rsb {

	// ------------------------------------------------

	const RSBPackAndUnpackModeE = [
		'group',
		'subgroup',
		'resource',
	] as const;

	type RSBPackAndUnpackMode = typeof RSBPackAndUnpackModeE[number];

	function make_rsb_package_relative_path(
		mode: RSBPackAndUnpackMode,
	) {
		let result: {
			resource_directory: Executor.RequireArgument<string>;
			packet_file: Executor.RequireArgument<string>;
			packet_manifest_file: Executor.RequireArgument<string>;
		};
		switch (mode) {
			case 'group': {
				result = {
					resource_directory: 'group/{0}/{1}/resource',
					packet_file: 'group/{0}/{1}/packet.rsgp',
					packet_manifest_file: 'group/{0}/{1}/manifest.json',
				};
				break;
			}
			case 'subgroup': {
				result = {
					resource_directory: 'subgroup/{1}/resource',
					packet_file: 'subgroup/{1}/packet.rsgp',
					packet_manifest_file: 'subgroup/{1}/manifest.json',
				};
				break;
			}
			case 'resource': {
				result = {
					resource_directory: 'resource',
					packet_file: 'packet/{1}.rsgp',
					packet_manifest_file: 'packet/{1}.json',
				};
				break;
			}
		}
		return result;
	}

	// ------------------------------------------------

	type ResourceExtractOption = {
		json: Executor.RequestArgument<boolean, false>;
		json_crypt: Executor.RequestArgument<boolean, false>;
		json_crypt_key: Executor.RequestArgument<string, false>;
		image: Executor.RequestArgument<boolean, false>;
		image_texture_format_map_list: Record<string, Support.PvZ2.RSB.ResourceExtract.TextureFormatMap>;
		image_texture_format_map_name: Executor.RequestArgument<string, false>;
		image_atlas: Executor.RequestArgument<boolean, false>;
		image_atlas_resize: Executor.RequestArgument<boolean, false>;
		image_sprite: Executor.RequestArgument<boolean, false>;
		animation: Executor.RequestArgument<boolean, false>;
		animation_json: Executor.RequestArgument<boolean, false>;
		animation_flash: Executor.RequestArgument<boolean, false>;
		audio: Executor.RequestArgument<boolean, false>;
		audio_tool: {
			ffmpeg_program: string;
			ww2ogg_program: string;
			ww2ogg_code_book: string;
		};
	};

	// ------------------------------------------------

	type Config = {
		pack_buffer_size: Executor.RequestArgument<string, false>;
		resource_extract_option: ResourceExtractOption;
	};

	export function _injector(
		config: Config,
	) {
		g_executor_method.push(
			Executor.method_of({
				id: 'popcap.rsb.pack',
				descriptor(
				) {
					return Executor.query_method_description(this.id);
				},
				worker(a: Entry.CFSA & {
					bundle_directory: Executor.RequireArgument<string>;
					data_file: Executor.RequestArgument<string, true>;
					mode: Executor.RequestArgument<string, false>;
					version_number: Executor.RequestArgument<bigint, false>;
					version_additional_texture_information_for_pvz_2_chinese_android: bigint | '?input';
					buffer_size: Executor.RequestArgument<string, false>;
					input_packet: Executor.RequestArgument<boolean, false>;
					output_new_packet: Executor.RequestArgument<boolean, false>;
				}) {
					let bundle_directory: string;
					let data_file: string;
					let mode: RSBPackAndUnpackMode;
					let version_number: [3n, 4n][number];
					let version_additional_texture_information_for_pvz_2_chinese_android: [0n, 1n, 2n][number];
					let buffer_size: bigint;
					let input_packet: boolean;
					let output_new_packet: boolean;
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
							() => (bundle_directory.replace(/((\.rsb)(\.bundle))?$/i, '.rsb')),
							...Executor.argument_requester_for_path('file', [false, a.fs_tactic_if_exist]),
						);
						mode = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'mode'),
							a.mode,
							(value) => (value),
							null,
							() => (Console.option([
								[RSBPackAndUnpackModeE[0], localized(`群组：按群+子群树形结构导入，资源与子包均导入自group/<群名>/<子群名>目录`)],
								[RSBPackAndUnpackModeE[1], localized(`子群：按子群树形结构导入，资源与子包均导入自subgroup/<子群名>目录`)],
								[RSBPackAndUnpackModeE[2], localized(`资源：所有资源导入自resource目录，所有子包导入自packet目录`)],
							], null)),
							(value) => (RSBPackAndUnpackModeE.includes(value as any) ? null : localized(`选项非法`)),
						);
						version_number = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'version_number'),
							a.version_number,
							(value) => (value),
							null,
							() => (Console.option([0n, null, null, null, [3n, ''], [4n, '']], null)),
							(value) => ([3n, 4n].includes(value) ? null : localized(`版本不受支持`)),
						);
						version_additional_texture_information_for_pvz_2_chinese_android = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'version_additional_texture_information_for_pvz_2_chinese_android'),
							a.version_additional_texture_information_for_pvz_2_chinese_android,
							(value) => (value),
							null,
							() => (Console.integer(null)),
							(value) => ([0n, 1n, 2n].includes(value) ? null : localized(`版本不受支持`)),
						);
						buffer_size = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'buffer_size'),
							a.buffer_size,
							(value) => (parse_size_string(value)),
							null,
							() => (Console.size(null)),
							(value) => (null),
						);
						input_packet = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'input_packet'),
							a.input_packet,
							(value) => (value),
							null,
							() => (Console.confirm(null)),
							(value) => (null),
						);
						output_new_packet = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'output_new_packet'),
							a.output_new_packet,
							(value) => (value),
							null,
							() => (Console.confirm(null)),
							(value) => (null),
						);
					}
					let relative_path = make_rsb_package_relative_path(mode);
					let manifest_file = `${bundle_directory}/manifest.json`;
					let description_file = `${bundle_directory}/description.json`;
					let resource_directory = `${bundle_directory}/${relative_path.resource_directory}`;
					let packet_file = !input_packet ? null : `${bundle_directory}/${relative_path.packet_file}`;
					let new_packet_file = !output_new_packet ? null : `${bundle_directory}/${relative_path.packet_file}`;
					CoreX.Tool.PopCap.RSB.pack_fs(data_file, manifest_file, description_file, resource_directory, packet_file, new_packet_file, { number: version_number, additional_texture_information_for_pvz_2_chinese_android: version_additional_texture_information_for_pvz_2_chinese_android }, buffer_size);
					Console.notify('s', localized(`执行成功`), [`${data_file}`]);
				},
				default_argument: {
					...Entry.k_cfsa,
					bundle_directory: undefined!,
					data_file: '?default',
					mode: '?input',
					version_number: '?input',
					version_additional_texture_information_for_pvz_2_chinese_android: '?input',
					buffer_size: config.pack_buffer_size,
					input_packet: '?input',
					output_new_packet: '?input',
				},
				input_filter: Entry.file_system_path_test_generator([['directory', /.+(\.rsb)(\.bundle)$/i]]),
				input_forwarder: 'bundle_directory',
			}),
			Executor.method_of({
				id: 'popcap.rsb.unpack',
				descriptor(
				) {
					return Executor.query_method_description(this.id);
				},
				worker(a: Entry.CFSA & {
					data_file: Executor.RequireArgument<string>;
					bundle_directory: Executor.RequestArgument<string, true>;
					mode: Executor.RequestArgument<string, false>;
					version_number: Executor.RequestArgument<bigint, false>;
					version_additional_texture_information_for_pvz_2_chinese_android: bigint | '?input';
					output_resource: Executor.RequestArgument<boolean, false>;
					output_packet: Executor.RequestArgument<boolean, false>;
				}) {
					let data_file: string;
					let bundle_directory: string;
					let mode: RSBPackAndUnpackMode;
					let version_number: [3n, 4n][number];
					let version_additional_texture_information_for_pvz_2_chinese_android: [0n, 1n, 2n][number];
					let output_resource: boolean;
					let output_packet: boolean;
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
							() => (data_file.replace(/((\.rsb))?$/i, '.rsb.bundle')),
							...Executor.argument_requester_for_path('directory', [false, a.fs_tactic_if_exist]),
						);
						mode = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'mode'),
							a.mode,
							(value) => (value),
							null,
							() => (Console.option([
								[RSBPackAndUnpackModeE[0], localized(`群组：按群+子群树形结构导出，资源与子包均导出至group/<群名>/<子群名>目录`)],
								[RSBPackAndUnpackModeE[1], localized(`子群：按子群树形结构导出，资源与子包均导出至subgroup/<子群名>目录`)],
								[RSBPackAndUnpackModeE[2], localized(`资源：所有资源导出至resource目录，所有子包导出至packet目录`)],
							], null)),
							(value) => (RSBPackAndUnpackModeE.includes(value as any) ? null : localized(`选项非法`)),
						);
						version_number = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'version_number'),
							a.version_number,
							(value) => (value),
							null,
							() => (Console.option([0n, null, null, null, [3n, ''], [4n, '']], null)),
							(value) => ([3n, 4n].includes(value) ? null : localized(`版本不受支持`)),
						);
						version_additional_texture_information_for_pvz_2_chinese_android = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'version_additional_texture_information_for_pvz_2_chinese_android'),
							a.version_additional_texture_information_for_pvz_2_chinese_android,
							(value) => (value),
							null,
							() => (Console.integer(null)),
							(value) => ([0n, 1n, 2n].includes(value) ? null : localized(`版本不受支持`)),
						);
						output_resource = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'output_resource'),
							a.output_resource,
							(value) => (value),
							null,
							() => (Console.confirm(null)),
							(value) => (null),
						);
						output_packet = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'output_packet'),
							a.output_packet,
							(value) => (value),
							null,
							() => (Console.confirm(null)),
							(value) => (null),
						);
					}
					let relative_path = make_rsb_package_relative_path(mode);
					let manifest_file = `${bundle_directory}/manifest.json`;
					let description_file = `${bundle_directory}/description.json`;
					let resource_directory = !output_resource ? null : `${bundle_directory}/${relative_path.resource_directory}`;
					let packet_file = !output_packet ? null : `${bundle_directory}/${relative_path.packet_file}`;
					CoreX.Tool.PopCap.RSB.unpack_fs(data_file, manifest_file, description_file, resource_directory, packet_file, { number: version_number, additional_texture_information_for_pvz_2_chinese_android: version_additional_texture_information_for_pvz_2_chinese_android });
					Console.notify('s', localized(`执行成功`), [`${bundle_directory}`]);
				},
				default_argument: {
					...Entry.k_cfsa,
					data_file: undefined!,
					bundle_directory: '?default',
					mode: '?input',
					version_number: '?input',
					version_additional_texture_information_for_pvz_2_chinese_android: '?input',
					output_resource: '?input',
					output_packet: '?input',
				},
				input_filter: Entry.file_system_path_test_generator([['file', /.+(\.rsb)$/i]]),
				input_forwarder: 'data_file',
			}),
			Executor.method_of({
				id: 'popcap.rsb.extract_resource',
				descriptor(
				) {
					return Executor.query_method_description(this.id);
				},
				worker(a: Entry.CFSA & {
					data_file: Executor.RequireArgument<string>;
					bundle_directory: Executor.RequestArgument<string, true>;
					version_number: Executor.RequestArgument<bigint, false>;
					version_additional_texture_information_for_pvz_2_chinese_android: bigint | '?input';
					option: ResourceExtractOption;
				}) {
					let data_file: string;
					let bundle_directory: string;
					let version_number: [3n, 4n][number];
					let version_additional_texture_information_for_pvz_2_chinese_android: [0n, 1n, 2n][number];
					let option: Support.PvZ2.RSB.ResourceExtract.Option = {
						json: null,
						image: null,
						animation: null,
						audio: null,
					};
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
							() => (data_file.replace(/((\.rsb))?$/i, '.rsb.bundle')),
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
						version_additional_texture_information_for_pvz_2_chinese_android = Executor.request_argument(
							...Executor.query_argument_message(this.id, 'version_additional_texture_information_for_pvz_2_chinese_android'),
							a.version_additional_texture_information_for_pvz_2_chinese_android,
							(value) => (value),
							null,
							() => (Console.integer(null)),
							(value) => ([0n, 1n, 2n].includes(value) ? null : localized(`版本不受支持`)),
						);
						let extract_directory = `${bundle_directory}/extract`;
						{
							let json: boolean;
							json = Executor.request_argument(
								...Executor.query_argument_message(this.id, 'option_json'),
								a.option.json,
								(value) => (value),
								null,
								() => (Console.confirm(null)),
								(value) => (null),
							);
							if (json) {
								option.json = {
									directory: extract_directory,
									crypt: null,
								};
								let crypt: boolean;
								crypt = Executor.request_argument(
									...Executor.query_argument_message(this.id, 'option_json_crypt'),
									a.option.json_crypt,
									(value) => (value),
									null,
									() => (Console.confirm(null)),
									(value) => (null),
								);
								if (crypt) {
									let key: string;
									key = Executor.request_argument(
										...Executor.query_argument_message(this.id, 'option_json_crypt_key'),
										a.option.json_crypt_key,
										(value) => (value),
										null,
										() => (Console.string(null)),
										(value) => (null),
									);
									option.json.crypt = {
										key: key,
									};
								}
							}
						}
						{
							let image: boolean;
							image = Executor.request_argument(
								...Executor.query_argument_message(this.id, 'option_image'),
								a.option.image,
								(value) => (value),
								null,
								() => (Console.confirm(null)),
								(value) => (null),
							);
							if (image) {
								option.image = {
									directory: extract_directory,
									texture_format_map: null!,
									atlas: null,
									sprite: null,
								};
								{
									let map_name_list = Object.keys(a.option.image_texture_format_map_list);
									if (map_name_list.length === 0) {
										throw new Error(`texture format map list is empty`);
									}
									let texture_format_map_name: string;
									texture_format_map_name = Executor.request_argument(
										...Executor.query_argument_message(this.id, 'option_image_texture_format_map_name'),
										a.option.image_texture_format_map_name,
										(value) => (value),
										null,
										() => (Console.option(map_name_list.map((e) => ([e])), null)),
										(value) => (map_name_list.includes(value) ? null : localized(`选项非法`)),
									);
									option.image.texture_format_map = a.option.image_texture_format_map_list[texture_format_map_name];
								}
								let atlas: boolean;
								atlas = Executor.request_argument(
									...Executor.query_argument_message(this.id, 'option_image_atlas'),
									a.option.image_atlas,
									(value) => (value),
									null,
									() => (Console.confirm(null)),
									(value) => (null),
								);
								let sprite: boolean;
								sprite = Executor.request_argument(
									...Executor.query_argument_message(this.id, 'option_image_sprite'),
									a.option.image_sprite,
									(value) => (value),
									null,
									() => (Console.confirm(null)),
									(value) => (null),
								);
								if (atlas) {
									let resize: boolean;
									resize = Executor.request_argument(
										...Executor.query_argument_message(this.id, 'option_image_atlas_resize'),
										a.option.image_atlas_resize,
										(value) => (value),
										null,
										() => (Console.confirm(null)),
										(value) => (null),
									);
									option.image.atlas = {
										resize: resize,
									};
								}
								if (sprite) {
									option.image.sprite = {};
								}
							}
						}
						{
							let animation: boolean;
							animation = Executor.request_argument(
								...Executor.query_argument_message(this.id, 'option_animation'),
								a.option.animation,
								(value) => (value),
								null,
								() => (Console.confirm(null)),
								(value) => (null),
							);
							if (animation) {
								option.animation = {
									directory: extract_directory,
									json: null,
									flash: null,
								};
								let json: boolean;
								json = Executor.request_argument(
									...Executor.query_argument_message(this.id, 'option_animation_json'),
									a.option.animation_json,
									(value) => (value),
									null,
									() => (Console.confirm(null)),
									(value) => (null),
								);
								let flash: boolean;
								flash = Executor.request_argument(
									...Executor.query_argument_message(this.id, 'option_animation_flash'),
									a.option.animation_flash,
									(value) => (value),
									null,
									() => (Console.confirm(null)),
									(value) => (null),
								);
								if (json) {
									option.animation.json = {};
								}
								if (flash) {
									option.animation.flash = {};
								}
							}
						}
						{
							let audio: boolean;
							audio = Executor.request_argument(
								...Executor.query_argument_message(this.id, 'option_audio'),
								a.option.audio,
								(value) => (value),
								null,
								() => (Console.confirm(null)),
								(value) => (null),
							);
							if (audio) {
								option.audio = {
									directory: extract_directory,
									tool: {
										ffmpeg_program: Main.path_at_home(a.option.audio_tool.ffmpeg_program),
										ww2ogg_program: Main.path_at_home(a.option.audio_tool.ww2ogg_program),
										ww2ogg_code_book: Main.path_at_home(a.option.audio_tool.ww2ogg_code_book),
									},
								};
							}
						}
					}
					Support.PvZ2.RSB.ResourceExtract.extract_package(
						data_file,
						`${bundle_directory}/manifest.json`,
						`${bundle_directory}/resource_manifest.json`,
						`${bundle_directory}/resource`,
						option,
						version_number,
						version_additional_texture_information_for_pvz_2_chinese_android,
					);
					Console.notify('s', localized(`执行成功`), [`${bundle_directory}`]);
				},
				default_argument: {
					...Entry.k_cfsa,
					data_file: undefined!,
					bundle_directory: '?default',
					version_number: '?input',
					version_additional_texture_information_for_pvz_2_chinese_android: '?input',
					option: config.resource_extract_option,
				},
				input_filter: Entry.file_system_path_test_generator([['file', /.+(\.rsb)$/i]]),
				input_forwarder: 'data_file',
			}),
		);
	}

	// ------------------------------------------------

}

({
	injector: TwinStar.Entry.method.popcap.rsb._injector,
});