# Enable Powerlevel10k instant prompt. Should stay close to the top of ~/.zshrc.
# Initialization code that may require console input (password prompts, [y/n]
# confirmations, etc.) must go above this block; everything else may go below.
if [[ -r "${XDG_CACHE_HOME:-$HOME/.cache}/p10k-instant-prompt-${(%):-%n}.zsh" ]]; then
  source "${XDG_CACHE_HOME:-$HOME/.cache}/p10k-instant-prompt-${(%):-%n}.zsh"
fi

# OSX antigen file
source ~/.antigen.zsh

if [[ -z "${ANTIGEN_LOADED}" ]]; then
  # Load the oh-my-zsh's library.
  antigen use oh-my-zsh

  # Load the theme
  antigen theme romkatv/powerlevel10k
  # antigen bundle mafredri/zsh-async
  # antigen bundle sindresorhus/pure@main

  # Bundles from the default repo (robbyrussell's oh-my-zsh).
  antigen bundle git
  antigen bundle command-not-found
  antigen bundle brew
  antigen bundle common-aliases
  antigen bundle compleat
  antigen bundle git-extras
  antigen bundle bazel
  antigen bundle npm
  antigen bundle macos
  antigen bundle zsh-users/zsh-history-substring-search
  antigen bundle zsh-users/zsh-autosuggestions
  antigen bundle zsh-users/zsh-syntax-highlighting

  # Tell Antigen that you're done.
  antigen apply
  export ANTIGEN_LOADED=1
fi

alias vim=nvim
alias code=codium
alias v=nvim
alias ..="cd .."
alias ...="cd ../.."
export EDITOR=nvim

# local scripts
export PATH=$PATH:~/.local/scripts

# clangd
export PATH="/usr/local/opt/llvm/bin:$PATH"

# Deno
export DENO_INSTALL="$HOME/.deno"
export PATH="$DENO_INSTALL/bin:$PATH"

# GO
export PATH="$HOME/go/bin:$PATH"

# Rust
export CARGO_NET_GIT_FETCH_WITH_CLI=true

# Kubectl
[[ -f /usr/local/bin/kubectl ]] && source <(kubectl completion zsh)

# Use a prefix with git-pile
# see https://github.com/keith/git-pile#optional
export GIT_PILE_PREFIX=pg/

# Open gh url
gh-pr-view(){
  gh pr view --web
} 

gh-pr-co(){
  SELECTED_PR=$(gh pr list | fzf)
  PR=$(echo $SELECTED_PR | awk '{print $1;}')
  gh pr checkout $PR
}

# Xcode via @orta
openx(){
  if test -n "$(find . -maxdepth 1 -name '*.xcworkspace' -print -quit)"
  then
    echo "Opening workspace"
    open *.xcworkspace
    return
  else
    if test -n "$(find . -maxdepth 1 -name '*.xcodeproj' -print -quit)"
    then
      echo "Opening project"
      open *.xcodeproj
      return
    else
      echo "Nothing found"
    fi
  fi
}

# bazel
alias bazel=bazelisk
# compdef bazelisk=bazel

# Envoy exports required for make_format 
export BUILDIFIER_BIN=$(which buildifier)
export BUILDOZER_BIN=$(which buildozer)
export CLANG_FORMAT=$(which clang-format)

# fzf config
[[ -f ~/.fzf.zsh ]] && source ~/.fzf.zsh

export FZF_CTRL_T_OPTS="--preview='bat --color=always --style=numbers {}' --bind ctrl-u:preview-page-up,ctrl-d:preview-page-down"
export FZF_DEFAULT_COMMAND='fd --type f --strip-cwd-prefix --hidden --exclude .git'
export FZF_CTRL_T_COMMAND='rg --no-messages --files'

# work config
[ -f ~/.private/init.zsh ] && source ~/.private/init.zsh

# To customize prompt, run `p10k configure` or edit ~/.p10k.zsh.
[[ ! -f ~/.p10k.zsh ]] || source ~/.p10k.zsh



