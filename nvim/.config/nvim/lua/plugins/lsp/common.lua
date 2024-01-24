local M = {}
local u = require('utils')

-- lsp commands
u.lua_command('LspDef', 'vim.lsp.buf.definition()')
u.lua_command('LspFormatting', 'vim.lsp.buf.format()')
u.lua_command('LspCodeAction', 'vim.lsp.buf.code_action()')
u.lua_command('LspHover', 'vim.lsp.buf.hover()')
u.lua_command('LspRename', 'vim.lsp.buf.rename()')
u.lua_command('LspRefs', 'vim.lsp.buf.references()')
u.lua_command('LspTypeDef', 'vim.lsp.buf.type_definition()')
u.lua_command('LspImplementation', 'vim.lsp.buf.implementation()')
u.lua_command('LspDiagPrev', 'vim.diagnostic.goto_prev()')
u.lua_command('LspDiagNext', 'vim.diagnostic.goto_next()')
u.lua_command('LspDiagLine', 'vim.diagnostic.open_float()')
u.lua_command('LspSignatureHelp', 'vim.lsp.buf.signature_help()')
u.lua_command('LspDiagQuickfix', 'vim.diagnostic.setqflist()')

-- default lsp mappings
local default_lsp_mappings = {
  ['gd'] = { cmd = ':LspDef<CR>', desc = 'Go to definition' },
  ['gf'] = { cmd = ':LspRefs<CR>', desc = 'Go to references' },
  ['gr'] = { cmd = ':LspRename<CR>', desc = 'Rename symbol' },
  ['gt'] = { cmd = ':LspTypeDef<CR>', desc = 'Go to type definition' },
  ['K'] = { cmd = ':LspHover<CR>', desc = 'Display hover informations' },
  ['H'] = { cmd = ':LspSignatureHelp<CR>', desc = 'Display signature' },
  ['[a'] = { cmd = ':LspDiagPrev<CR>', desc = 'Go to previous diagnostic' },
  [']a'] = { cmd = ':LspDiagNext<CR>', desc = 'Go to next diagnostic' },
  ['ga'] = { cmd = ':lua vim.lsp.buf.code_action()<CR>', desc = 'Display code actions' },
  ['gl'] = { cmd = ':LspDiagLine<CR>', desc = 'Display diagnostic line' },
  ['go'] = { cmd = ':Telescope lsp_references<CR>', desc = 'Display lsp references' },
}

M.format_on_save = function(client)
  if client.server_capabilities.documentFormattingProvider then
    vim.cmd([[
      augroup lsp_buf_format
        au! BufWritePre <buffer>
        autocmd BufWritePre <buffer> :lua vim.lsp.buf.format()
      augroup END
    ]])
  end
end

-- https://github.com/neovim/nvim-lspconfig/blob/master/doc/server_configurations.md
M.set_mappings = function(client, bufnr, nmap_mappings)
  local mappings = vim.tbl_extend('force', default_lsp_mappings, nmap_mappings or {})
  for key, item in pairs(mappings) do
    vim.api.nvim_buf_set_keymap(bufnr, 'n', key, item.cmd, { desc = item.desc, noremap = true, silent = true })
  end

  M.format_on_save(client)
  require('illuminate').on_attach(client)
end

return M
