#pragma once

#include "implement/common.hpp"
#include "implement/base_command.hpp"

#pragma warning(push)
#pragma warning(disable:4625)
#pragma warning(disable:4626)
#pragma warning(disable:5026)
#pragma warning(disable:5027)

namespace TwinStar::WindowsExplorerExtension {

	#pragma region config

	struct MethodInvokeCommandConfig {
		std::wstring                name;
		std::optional<bool>         type;
		std::optional<std::wregex>  rule;
		std::optional<std::wstring> method;
		std::wstring                argument;
	};

	struct MethodInvokeCommandConfigGroup {
		std::wstring                           name;
		std::vector<MethodInvokeCommandConfig> child;
		std::vector<std::size_t>               separator;
	};

	// ----------------

	inline auto test_single_path (
		MethodInvokeCommandConfig const & config,
		std::wstring const &              path
	) -> bool {
		auto result = true;
		if (config.type) {
			if (config.type.value()) {
				result &= std::filesystem::is_directory(std::filesystem::path{path});
			} else {
				result &= std::filesystem::is_regular_file(std::filesystem::path{path});
			}
		}
		if (result && config.rule) {
			result &= std::regex_search(path, config.rule.value());
		}
		return result;
	}

	#pragma endregion

	#pragma region command

	class MethodInvokeCommand :
		public BaseCommand {

	protected:

		MethodInvokeCommandConfig const & m_config;
		bool                              m_has_icon;

	public:

		#pragma region structor

		explicit MethodInvokeCommand (
			MethodInvokeCommandConfig const & config,
			bool const &                      has_icon = true
		):
			m_config{config},
			m_has_icon{has_icon} {
		}

		#pragma endregion

		#pragma region implement

		virtual auto title (
		) -> LPCWSTR override {
			return thiz.m_config.name.c_str();
		}

		virtual auto icon (
		) -> LPCWSTR override {
			static auto dll_path = get_module_file_name(g_dll_handle);
			return thiz.m_has_icon ? dll_path.data() : nullptr;
		}

		virtual auto state (
			_In_opt_ IShellItemArray * selection
		) -> EXPCMDSTATE override {
			if (selection == nullptr) {
				throw std::runtime_error{std::format("selection is null")};
			}
			auto path_list = get_shell_item_file_path(selection);
			for (auto & path : path_list) {
				if (!test_single_path(thiz.m_config, path)) {
					return ECS_DISABLED;
				}
			}
			return ECS_ENABLED;
		}

		virtual auto invoke (
			_In_opt_ IShellItemArray * selection
		) -> void override {
			try {
				if (selection == nullptr) {
					throw std::runtime_error{std::format("selection is null")};
				}
				auto path_list = get_shell_item_file_path(selection);
				auto program = std::wstring{L"C:\\Windows\\System32\\cmd.exe"};
				auto argument = std::vector<std::wstring>{};
				argument.emplace_back(L"/C");
				argument.emplace_back(L"%TwinStar.ToolKit.WindowsExplorerExtension.launch_file%");
				for (auto & path : path_list) {
					argument.emplace_back(path);
					if (thiz.m_config.method.has_value()) {
						argument.emplace_back(L"-method");
						argument.emplace_back(thiz.m_config.method.value());
					}
					argument.emplace_back(L"-argument");
					argument.emplace_back(thiz.m_config.argument);
				}
				create_process(program, argument);
			} catch (std::exception const & exception) {
				// TODO : suppose encoding is ANSI, right ?
				auto message = exception.what();
				MessageBoxA(nullptr, message, "TwinStar.ToolKit.WindowsExplorerExtension ERROR", MB_OK | MB_ICONERROR);
			}
			return;
		}

		#pragma endregion

	};

	class MethodInvokeCommandEnum :
		public RuntimeClass<RuntimeClassFlags<ClassicCom>, IEnumExplorerCommand> {

	protected:

		std::vector<ComPtr<IExplorerCommand>>                 m_commands;
		std::vector<ComPtr<IExplorerCommand>>::const_iterator m_current;

	public:

		#pragma region structor

		explicit MethodInvokeCommandEnum (
			MethodInvokeCommandConfigGroup const & config
		) {
			auto separator_index = std::size_t{0};
			auto current_separator_section_count = std::size_t{0};
			thiz.m_commands.reserve(config.child.size() + config.separator.size());
			for (auto & element : config.child) {
				if (separator_index < config.separator.size() && current_separator_section_count == config.separator[separator_index]) {
					current_separator_section_count = 0;
					thiz.m_commands.emplace_back(Make<SeparatorCommand>());
					++separator_index;
				}
				thiz.m_commands.emplace_back(Make<MethodInvokeCommand>(element, false));
				++current_separator_section_count;
			}
			thiz.m_current = thiz.m_commands.cbegin();
		}

		#pragma endregion

		#pragma region implement

		virtual IFACEMETHODIMP Next (
			ULONG                                                      celt,
			__out_ecount_part(celt, *pceltFetched) IExplorerCommand ** pUICommand,
			__out_opt ULONG *                                          pceltFetched
		) override {
			auto fetched = ULONG{0};
			wil::assign_to_opt_param(pceltFetched, 0ul);
			for (auto i = ULONG{0}; i < celt && thiz.m_current != thiz.m_commands.cend(); i++) {
				thiz.m_current->CopyTo(&pUICommand[0]);
				++thiz.m_current;
				++fetched;
			}
			wil::assign_to_opt_param(pceltFetched, fetched);
			return fetched == celt ? S_OK : S_FALSE;
		}

		virtual IFACEMETHODIMP Skip (
			ULONG celt
		) override {
			return E_NOTIMPL;
		}

		virtual IFACEMETHODIMP Reset (
		) override {
			thiz.m_current = thiz.m_commands.cbegin();
			return S_OK;
		}

		virtual IFACEMETHODIMP Clone (
			__deref_out IEnumExplorerCommand ** ppenum
		) override {
			*ppenum = nullptr;
			return E_NOTIMPL;
		}

		#pragma endregion

	};

	class MethodInvokeGroupCommand :
		public BaseCommand {

	protected:

		MethodInvokeCommandConfigGroup const & m_config;

	public:

		#pragma region structor

		explicit MethodInvokeGroupCommand (
			MethodInvokeCommandConfigGroup const & config
		):
			m_config{config} {
		}

		#pragma endregion

		#pragma region implement

		virtual IFACEMETHODIMP EnumSubCommands (
			_COM_Outptr_ IEnumExplorerCommand ** ppEnum
		) override {
			*ppEnum = nullptr;
			auto e = Make<MethodInvokeCommandEnum>(thiz.m_config);
			return e->QueryInterface(IID_PPV_ARGS(ppEnum));
		}

		// ----------------

		virtual auto title (
		) -> LPCWSTR override {
			return thiz.m_config.name.c_str();
		}

		virtual auto icon (
		) -> LPCWSTR override {
			static auto dll_path = get_module_file_name(g_dll_handle);
			return dll_path.data();
		}

		virtual auto state (
			_In_opt_ IShellItemArray * selection
		) -> EXPCMDSTATE override {
			if (selection == nullptr) {
				throw std::runtime_error{std::format("selection is null")};
			}
			auto path_list = get_shell_item_file_path(selection);
			for (auto & config : thiz.m_config.child) {
				auto state = true;
				for (auto & path : path_list) {
					if (!test_single_path(config, path)) {
						state = false;
						break;
					}
				}
				if (state) {
					return ECS_ENABLED;
				}
			}
			return ECS_DISABLED;
		}

		virtual auto flags (
		) -> EXPCMDFLAGS override {
			return ECF_HASSUBCOMMANDS;
		}

		virtual auto invoke (
			_In_opt_ IShellItemArray * selection
		) -> void override {
			return;
		}

		#pragma endregion

	};

	#pragma endregion

}

#pragma warning(pop)
