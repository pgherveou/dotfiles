#!/usr/bin/env bash

# Function to add a cron job
add_cron_job() {
	local script_path="$1"
	local time_schedule="$2"

	# Check if the cron job is already scheduled
	if crontab -l | grep -q "$script_path"; then
		echo "Cron job $script_path already scheduled."
	else
		# Add the cron job with the specified time schedule
		(
			crontab -l
			echo "$time_schedule $script_path"
		) | crontab -
		echo "Cron job $script_path scheduled with $time_schedule."
	fi
}

run_install() {
	set -euo pipefail
	pushd "$HOME/.dotfiles"

	# mac only
	if [[ "$OSTYPE" == "darwin"* ]]; then
		# keyboard repeat rate
		defaults write -g KeyRepeat -int 1
		defaults write -g InitialKevRepeat -int 13

		# install brew if running from macos
		if ! command -v brew &>/dev/null; then
			/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
		fi

		# ln -s /opt/homebrew/share/antigen/antigen.zsh ~/.antigen.zsh
		# brew bundle install
	fi

	# clone tmux plugin manager
	if [ ! -d ~/.tmux/plugins ]; then
		mkdir -p ~/.tmux/plugins
		git clone https://github.com/tmux-plugins/tpm ~/.tmux/plugins/tpm
	fi

	# clone qmk firmware
	if [ ! -d ~/qmk_firmware ]; then
		git clone https://github.com/qmk/qmk_firmware ~/qmk_firmware
		qmk setup -y
		qmk config user.keyboard=splitkb/kyria/rev1
		qmk config user.keymap=pgherveou
		qmk generate-compilation-database
		ln -s ~/qmk_firmware/compile_commands.json "$PWD"
	fi

	# create deeplinks to the home folder
	STOW_FOLDERS=(
		"alacritty"
		"git"
		"ideavim"
		"nvim"
		"codespell"
		"private"
		"tmux"
		"qmk"
		"zsh"
		"karabiner"
		"bin"
		"hammerspoon"
		"cargo"
		"ra-multiplex"
	)

	for folder in "${STOW_FOLDERS[@]}"; do
		echo "Stowing $folder"
		stow -D "$folder"
		stow "$folder"
	done

	# list of globally installed npm packages
	NPM_PKGS=(
		"neovim"
		"serve"
		"speed-test"
	)

	# install global npm packages using sudo on linux
	if [[ "$OSTYPE" == "linux-gnu"* ]]; then
		sudo npm install -g "${NPM_PKGS[@]}"
	else
		npm install -g "${NPM_PKGS[@]}"
	fi

	# install cargo binaries
	CARGO_BIN=(
		"bacon"
		"cargo-watch"
		"evcxr"
		"silicon"
	)

	for bin in "${CARGO_BIN[@]}"; do
		cargo install "$bin"
	done

	# Add cron jobs

	# cleanup rust projects every day at 7am
	add_cron_job "$HOME/.dotfiles/bin/.local/scripts/rust_projects_cleanup.sh" "0 7 * * *"

	pop
}

# Run install when the script is executed
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
	run_install
fi
