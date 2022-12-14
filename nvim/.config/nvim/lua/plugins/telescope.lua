local M = {}
M.file_browser = function()
  local cwd = require('telescope.utils').buffer_dir()

  -- https://github.com/nvim-telescope/telescope-file-browser.nvim#mappings
  require('telescope').extensions.file_browser.file_browser({
    cwd = cwd,
    follow = true,
    hidden = true,
    no_ignore = true,
  })
end

M.find_files = function()
  require('telescope.builtin').find_files({
    follow = true,
    hidden = true,
  })
end

M.setup = function()
  local telescope = require('telescope')
  local themes = require('telescope.themes')
  local actions = require('telescope.actions')
  local fb_actions = require('telescope').extensions.file_browser.actions

  telescope.setup({
    ['ui-select'] = {
      themes.get_dropdown(),
    },
    defaults = {
      -- path_display = { truncate = 3 },
      file_ignore_patterns = { '.git/' },
      -- https://github.com/nvim-telescope/telescope.nvim/blob/master/lua/telescope/mappings.lua#L9
      -- https://github.com/nvim-telescope/telescope.nvim#default-mappings

      layout_config = {
        horizontal = { width = 0.95 },
      },

      mappings = {
        i = {
          ['<C-n>'] = actions.cycle_history_next,
          ['<C-p>'] = actions.cycle_history_prev,
          ['<C-c>'] = actions.close,
          ['<c-x>'] = actions.delete_buffer,
        },
        n = {
          ['q'] = actions.close,
          ['x'] = actions.delete_buffer,
          ['d'] = fb_actions.remove,
        },
      },
    },
    pickers = {
      buffers = {
        path_display = { truncate = 3 },
      },
      find_files = {
        path_display = { truncate = 3 },
      },
      file_browser = {
        path_display = { truncate = 3 },
      },
    },
  })

  telescope.load_extension('fzf')
  telescope.load_extension('file_browser')
  telescope.load_extension('gh')
  telescope.load_extension('harpoon')
  telescope.load_extension('git_worktree')
  telescope.load_extension('ui-select')
  telescope.load_extension('refactoring')

  -- stylua: ignore start
  local mappings = {
    ['<Leader>fb'] = { mode = 'n', cmd = ':lua require("telescope.builtin").buffers{}<CR>', desc = 'Search buffers' },
    ['<Leader>fe'] = { mode = 'n', cmd = ':Telescope file_browser hidden=true path=%:p:h respect_gitignore=false<CR>', desc = 'Open file browser', },
    ['<Leader>ff'] = { mode = 'n', cmd = ':lua require("plugins.telescope").find_files{}<CR>', desc = 'Search files' },
    ['<Leader>fg'] = { mode = 'n', cmd = ':lua require("telescope.builtin").live_grep{}<CR>', desc = 'Search with Live grep' },
    ['<leader>fs'] = { mode = 'v', cmd = '"zy:Telescope grep_string search=<C-r>z<CR>', desc = 'Search from selection' },
    ['<Leader>fs'] = { mode = 'n', cmd = ':lua require("telescope.builtin").grep_string{}<CR>', desc = 'Search from word under cursor' },
    ['<Leader>fc'] = { mode = 'n', cmd = ':lua require("telescope.builtin").command_history{}<CR>', desc = 'Search command history', },
    ['<Leader>fh'] = { mode = 'n', cmd = ':lua require("telescope.builtin").search_history{}<CR>', desc = 'Search history', },
    ['<Leader>fo'] = { mode = 'n', cmd = ':lua require("telescope.builtin").oldfiles{}<CR>', desc = 'Search recent files', },
    ['<Leader>fr'] = { mode = 'n', cmd = ':Telescope resume<CR>', desc = 'Resume search' },
    ['<leader>f/'] = { mode = 'n', cmd = ':lua require("telescope").extensions.live_grep_args.live_grep_args()<CR>', desc = 'Search with raw grep', },
    ['<leader>wt'] = { mode = 'n', cmd = ':lua require("telescope").extensions.git_worktree.git_worktrees()<CR>', desc = 'Search git worktree', },
    ['<leader>cw'] = { mode = 'n', cmd = ':lua require("telescope").extensions.git_worktree.create_git_worktree()<CR>', desc = 'Create git worktree', },
    ['<leader>rr'] = { mode = 'v', cmd = ':lua require("telescope").extensions.refactoring.refactors()<CR>', desc = 'Search refactors' },
  }
  -- stylua: ignore end
  for key, item in pairs(mappings) do
    vim.api.nvim_set_keymap(item.mode, key, item.cmd, { desc = item.desc, noremap = true, silent = true })
  end
end

return M
