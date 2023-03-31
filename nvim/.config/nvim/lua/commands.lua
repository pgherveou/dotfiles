vim.api.nvim_create_user_command('ClearRegisters', function()
  local regs = vim.split('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/-"', '')
  for _, r in ipairs(regs) do
    vim.fn.setreg(r, {})
  end
end, {})

-- rename a file
vim.api.nvim_create_user_command('Rename', function()
  local old_name = vim.fn.expand('%')
  local new_name = vim.fn.input('New name: ', old_name, 'file')
  if new_name ~= '' then
    vim.fn.rename(old_name, new_name)
    vim.cmd('edit ' .. new_name)
  end
end, {})

-- delete a file
vim.api.nvim_create_user_command('Delete', function()
  local file = vim.fn.expand('%')
  local confirm = vim.fn.input('Delete ' .. file .. '? (y/N) ')
  if confirm == 'y' then
    vim.cmd('bd')
    vim.fn.delete(file)
  end
end, {})

-- reload the current file by calling :BufDel first
vim.api.nvim_create_user_command('BufReload', function()
  local current_file = vim.fn.expand('%')
  local current_position = vim.fn.getpos('.')

  vim.cmd(':bd')
  vim.cmd('edit ' .. current_file)
  vim.fn.setpos('.', current_position)
end, {})
