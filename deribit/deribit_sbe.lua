--
-- Wireshark dissector for Deribit SBE Multicast protocol
-- In order to use it you have to put this script in $HOME/.local/lib/wireshark/plugins folder
--

local proto_sbe = Proto("deribit_sbe", "Deribit SBE")

local f_packet_length = ProtoField.uint16("deribit_sbe.packet_length", "Packet Length", base.DEC)
local f_channel_id = ProtoField.uint16("deribit_sbe.channel_id", "Channel Id", base.DEC)
local f_channel_seq = ProtoField.uint32("deribit_sbe.channel_seq", "Channel Sequence", base.DEC)

local msg_types = {
	[1000] = "instrument",
	[1001] = "book",
	[1002] = "trades",
	[1003] = "ticker",
	[1004] = "snapshot"
}

local book_sides = {
	[0] = "ask",
	[1] = "bid"
}

local book_changes = {
	[0] = "new",
	[1] = "change",
	[2] = "delete"
}

local trade_direction = {
	[0] = "buy",
	[1] = "sell"
}

local liquidation_side = {
	[0] = "no liquidation",
	[1] = "maker",
	[2] = "taker",
	[3] = "maker"
}

local tick_direction = {
	[0] = "plus",
	[1] = "zeroplus",
	[2] = "minus",
	[3] = "zerominus"
}

local instrument_state = {
	[0] = "created",
	[1] = "open",
	[2] = "closed",
	[3] = "settled"
}

local instrument_kind = {
	[0] = "future",
	[1] = "option"
}

local future_type = {
	[0] = "not applicable",
	[1] = "reversed",
	[2] = "linear"
}

local option_type = {
	[0] = "not applicable",
	[1] = "call",
	[2] = "put"
}

local period = {
	[0] = "perpetual",
	[1] = "minute",
	[2] = "hour",
	[3] = "day",
	[4] = "week",
	[5] = "month",
	[6] = "year"
}

local last_part = {
	[0] = "more to come",
	[1] = "last"
}

local book_complete = {
	[0] = "incomplete (depth limited)",
	[1] = "complete"
}

local f_template_id = ProtoField.uint16("deribit_sbe.template_id", "Type", base.DEC, msg_types)
local f_schema_id = ProtoField.uint16("deribit_sbe.schema_id", "Schema Id", base.DEC)
local f_version = ProtoField.uint16("deribit_sbe.version", "Version", base.DEC)
local f_block_length = ProtoField.uint16("deribit_sbe.block_length", "Root Block Length", base.DEC)
local f_num_groups = ProtoField.uint16("deribit_sbe.num_groups", "Num Groups", base.DEC)
local f_num_vars = ProtoField.uint16("deribit_sbe.num_vars", "Num Vars", base.DEC)
local f_instrument_id = ProtoField.uint32("deribit_sbe.instrument_id", "Instrument Id", base.DEC)

local f_group_block_len = ProtoField.uint16("deribit_sbe.group_block_len", "Group Block Length", base.DEC)
local f_group_num_in_group = ProtoField.uint16("deribit_sbe.num_in_group", "Num In Group", base.DEC)
local f_group_num_groups = ProtoField.uint16("deribit_sbe.group_num_groups", "Group Num Groups", base.DEC)
local f_group_num_vars = ProtoField.uint16("deribit_sbe.group_num_vars", "Group Num Vars", base.DEC)

local f_var_len = ProtoField.uint8("deribit_sbe.var_length", "Variable Length", base.DEC)
local f_var_bytes = ProtoField.bytes("deribit_sbe.var_data", "Variable Data")

local f_timestamp = ProtoField.absolute_time("deribit_sbe.timestamp", "Timestamp", base.UTC)
local f_change_id = ProtoField.uint64("deribit_sbe.change_id", "Change ID", base.DEC)
local f_prev_change_id = ProtoField.uint64("deribit_sbe.prev_change_id", "Previous Change ID", base.DEC)
local f_price = ProtoField.double("deribit_sbe.price", "Price")
local f_amount = ProtoField.double("deribit_sbe.amount", "Amount")
local f_book_side = ProtoField.uint8("deribit_sbe.book_side", "Side", base.DEC, book_sides)
local f_book_change = ProtoField.uint8("deribit_sbe.book_change", "Change", base.DEC, book_changes)
local f_instrument_name = ProtoField.string("deribit_sbe.instrument_name", "Instrument Name")
local f_is_book_complete = ProtoField.uint8("deribit_sbe.is_book_complete", "Is Book Complete", base.DEC, book_complete)
local f_is_last_part = ProtoField.uint8("deribit_sbe.is_last_part", "Is Last Part", base.DEC, last_part)

local f_trade_direction = ProtoField.uint8("deribit_sbe.trade_direction", "Trade Direction", base.DEC, trade_direction)
local f_tick_direction = ProtoField.uint8("deribit_sbe.tick_direction", "Tick Direction", base.DEC, tick_direction)
local f_liquidation = ProtoField.uint8("deribit_sbe.liquidation", "Liquidation", base.DEC, liquidation_side)
local f_mark_price = ProtoField.double("deribit_sbe.mark_price", "Mark Price")
local f_index_price = ProtoField.double("deribit_sbe.index_price", "Index Price")
local f_trade_seq = ProtoField.uint64("deribit_sbe.trade_seq", "Trade Sequence", base.DEC)
local f_trade_id = ProtoField.uint64("deribit_sbe.trade_id", "Trade Id", base.DEC)
local f_iv = ProtoField.double("deribit_sbe.iv", "Implied Volatility")
local f_block_trade_id = ProtoField.uint64("deribit_sbe.block_trade_id", "Block Trade Id", base.DEC)
local f_combo_trade_id = ProtoField.uint64("deribit_sbe.combo_trade_id", "Combo Trade Id", base.DEC)
local f_instrument_state = ProtoField.uint8("deribit_sbe.instrument_state", "Instrument State", base.DEC, instrument_state)
local f_instrument_kind = ProtoField.uint8("deribit_sbe.instrument_kind", "Instrument Kind", base.DEC, instrument_kind)
local f_period = ProtoField.uint8("deribit_sbe.period", "Settlement Period", base.DEC, period)
local f_period_count = ProtoField.uint16("deribit_sbe.period_count", "Settlement Period Count", base.DEC)
local f_option_type = ProtoField.uint8("deribit_sbe.option_type", "Option Type", base.DEC, option_type)
local f_future_type = ProtoField.uint8("deribit_sbe.future_type", "Future Type", base.DEC, future_type)
local f_rfq = ProtoField.uint8("deribit_sbe.rfq", "RFQ", base.DEC)
local f_base_currency = ProtoField.string("deribit_sbe.base_currency", "Base Currency")
local f_quote_currency = ProtoField.string("deribit_sbe.quote_currency", "Quote Currency")
local f_counter_currency = ProtoField.string("deribit_sbe.counter_currency", "Counter Currency")
local f_settlement_currency = ProtoField.string("deribit_sbe.settlement_currency", "Settlement Currency")
local f_size_currency = ProtoField.string("deribit_sbe.size_currency", "Size Currency")
local f_creation_timestamp = ProtoField.absolute_time("deribit_sbe.creation_timestamp", "Creation Timestamp", base.UTC)
local f_expiration_timestamp = ProtoField.absolute_time("deribit_sbe.expiration_timestamp", "Expiration Timestamp", base.UTC)
local f_strike_price = ProtoField.double("deribit_sbe.strike_price", "Strike Price")
local f_contract_size = ProtoField.double("deribit_sbe.contract_size", "Contract Size")
local f_min_trade_amount = ProtoField.double("deribit_sbe.min_trade_amount", "Minimum Trade Amount")
local f_tick_size = ProtoField.double("deribit_sbe.tick_size", "Tick Size")
local f_maker_commission = ProtoField.double("deribit_sbe.maker_commission", "Maker Commission")
local f_taker_commission = ProtoField.double("deribit_sbe.taker_commission", "Taker Commission")
local f_block_trade_commission = ProtoField.double("deribit_sbe.block_trade_commission", "Block Trade Commission")
local f_max_liquidation_commission = ProtoField.double("deribit_sbe.max_liquidation_commission", "Max Liquidation Commission")
local f_max_leverage = ProtoField.double("deribit_sbe.max_leverage", "Max Leverage")
local f_open_interest = ProtoField.double("deribit_sbe.open_interest", "Open Interest")
local f_min_sell_price = ProtoField.double("deribit_sbe.min_sell_price", "Min Sell Price")
local f_max_buy_price = ProtoField.double("deribit_sbe.max_buys_price", "Max Buy Price")
local f_last_price = ProtoField.double("deribit_sbe.last_price", "Last Price")
local f_current_funding = ProtoField.double("deribit_sbe.current_funding", "Current Funding")
local f_funding_8h = ProtoField.double("deribit_sbe.funding_8h", "Funding 8h")
local f_estimated_delivery_price = ProtoField.double("deribit_sbe.estimated_delivery_price", "Estimated Delivery Price")
local f_delivery_price = ProtoField.double("deribit_sbe.delivery_price", "Delivery Price")
local f_settlement_price = ProtoField.double("deribit_sbe.settlement_price", "Settlement Price")
local f_best_bid_price = ProtoField.double("deribit_sbe.best_bid_price", "Best Bid Price")
local f_best_bid_amount = ProtoField.double("deribit_sbe.best_bid_amount", "Best Bid Amount")
local f_best_ask_price = ProtoField.double("deribit_sbe.best_ask_price", "Best Ask Price")
local f_best_ask_amount = ProtoField.double("deribit_sbe.best_ask_amount", "Best Ask Amount")


-- all fieds have to be registered here
proto_sbe.fields = {
	f_packet_length,
	f_channel_id,
	f_channel_seq,

	f_template_id,
	f_instrument_id,
	f_schema_id,
	f_version,
	f_block_length,
	f_num_groups,
	f_num_vars,

	f_group_block_len,
	f_group_num_in_group,
	f_group_num_groups,
	f_group_num_vars,

	f_var_len,
	f_var_bytes,

	f_timestamp,
	f_change_id,
	f_prev_change_id,
	f_price,
	f_amount,
	f_book_side,
	f_book_change,
	f_is_book_complete,
	f_is_last_part,
	f_instrument_name,

	f_trade_direction,
	f_tick_direction,
	f_liquidation,
	f_mark_price,
	f_index_price,
	f_trade_seq,
	f_trade_id,
	f_iv,
	f_block_trade_id,
	f_combo_trade_id,

	f_instrument_state,
	f_instrument_kind,
	f_option_type,
	f_future_type,
	f_rfq,
	f_period,
	f_period_count,
	f_base_currency,
	f_quote_currency,
	f_counter_currency,
	f_settlement_currency,
	f_size_currency,
	f_creation_timestamp,
	f_expiration_timestamp,
	f_strike_price,
	f_contract_size,
	f_min_trade_amount,
	f_tick_size,
	f_maker_commission,
	f_taker_commission,
	f_block_trade_commission,
	f_max_leverage,
	f_max_liquidation_commission,

	f_open_interest,
	f_min_sell_price,
	f_max_buy_price,
	f_last_price,
	f_current_funding,
	f_funding_8h,
	f_estimated_delivery_price,
	f_delivery_price,
	f_settlement_price,
	f_best_bid_price,
	f_best_bid_amount,
	f_best_ask_price,
	f_best_ask_amount
}

-- the `dissector()` method is called by Wireshark when parsing our packets
function proto_sbe.dissector(buffer, pinfo, tree)
	pinfo.cols.protocol = "Deribit SBE"
	local subtree = tree:add(proto_sbe, buffer())
	local frameHeader = subtree:add(proto_sbe, buffer(0, 8), "Framing Header")
	frameHeader:add_le(f_packet_length, buffer(0,2))
	frameHeader:add_le(f_channel_id, buffer(2,2))
	frameHeader:add_le(f_channel_seq, buffer(4,4))
	local info = string.format("Channel Id: %-2d Seq: %-6d", buffer(2,2):le_uint(), buffer(4,4):le_uint())
	local offset = 8
	repeat
		local type = buffer(offset + 2,2):le_uint()
		if type == 1000 then
			offset = instrument(buffer, offset, subtree)
			info = info .. " [instrument]"
		elseif type == 1001 then
			offset = book(buffer, offset, subtree)
			info = info .. " [book]"
		elseif type == 1002 then
			offset = trades(buffer, offset, subtree)
			info = info .. " [trades]"
		elseif type == 1003 then
			offset = ticker(buffer, offset, subtree)
			info = info .. " [ticker]"
		elseif type == 1004 then
			offset = snapshot(buffer, offset, subtree)
			info = info .. " [snapshot]"
		else
			-- messed up somewhere ... make it stop
			offset = offset + 100000
			info = info .. " <unknown>"
		end
	until (offset >= buffer:len())
	pinfo.cols.info = info
end

function instrument(buffer, ioffset, subtree)
	local offset = ioffset
	local message = subtree:add(proto_sbe, buffer(ioffset,16), "Instrument State")

	offset = message_header(buffer, offset, message)

	message:add_le(f_instrument_id, buffer(offset, 4))
	message:add(f_instrument_state, buffer(offset + 4, 1))
	message:add(f_instrument_kind, buffer(offset + 5, 1))
	message:add(f_future_type, buffer(offset + 6, 1))
	message:add(f_option_type, buffer(offset + 7, 1))
	message:add(f_rfq, buffer(offset + 8, 1))
	message:add(f_period, buffer(offset + 9, 1))
	message:add_le(f_period_count, buffer(offset + 10, 2))
	offset = currency(buffer, offset + 12, message, f_base_currency)
	offset = currency(buffer, offset, message, f_quote_currency)
	offset = currency(buffer, offset, message, f_counter_currency)
	offset = currency(buffer, offset, message, f_settlement_currency)
	offset = currency(buffer, offset, message, f_size_currency)
	offset = timestamp(buffer, offset, message, f_creation_timestamp)
	offset = timestamp(buffer, offset, message, f_expiration_timestamp)
	message:add_le(f_strike_price, buffer(offset, 8))
	message:add_le(f_contract_size, buffer(offset + 8, 8))
	message:add_le(f_min_trade_amount, buffer(offset + 16, 8))
	message:add_le(f_tick_size, buffer(offset + 24, 8))
	message:add_le(f_maker_commission, buffer(offset + 32, 8))
	message:add_le(f_taker_commission, buffer(offset + 40, 8))
	message:add_le(f_block_trade_commission, buffer(offset + 48, 8))
	message:add_le(f_max_liquidation_commission, buffer(offset + 56, 8))
	message:add_le(f_max_leverage, buffer(offset + 64, 8))

	local var_size = buffer(offset + 72, 1):uint()
	local var = message:add(f_instrument_name, buffer(offset + 73, var_size))
	offset = offset + 73 + var_size

	return offset

end

function book(buffer, ioffset, subtree)
	local offset = ioffset
	local message = subtree:add(proto_sbe, buffer(ioffset,16), "Book Change")

	offset = message_header(buffer, offset, message)

	message:add_le(f_instrument_id, buffer(offset,4))

	offset = timestamp(buffer, offset + 4, message, f_timestamp)

	message:add_le(f_prev_change_id, buffer(offset ,8))
	message:add_le(f_change_id, buffer(offset + 8, 8))
	message:add(f_is_last_part, buffer(offset + 16, 1))

	offset = offset + 17

	local group_block_len = buffer(offset, 2):le_uint()
	local group_num_in_group = buffer(offset + 2, 2):le_uint()
	local group_size = group_block_len * group_num_in_group
	local change_group = message:add(proto_sbe, buffer(offset, 8 + group_size), "Changes")
	local group_header = change_group:add(proto_sbe, buffer(offset, 8 + group_size), "Group Header")


	group_header:add_le(f_group_block_len, buffer(offset, 2))
	group_header:add_le(f_group_num_in_group, buffer(offset + 2, 2))
	group_header:add_le(f_group_num_groups, buffer(offset + 4, 2))
	group_header:add_le(f_group_num_vars, buffer(offset + 6, 2))

	offset = offset + 8

	local change_list = change_group:add(proto_sbe, buffer(offset, group_size), "Change List")

	for i=1, group_num_in_group do
		local side = book_sides[buffer(offset, 1):uint()]
		local change = book_changes[buffer(offset + 1, 1):uint()]
		local price = buffer(offset + 2, 8):le_float()
		local amount = buffer(offset + 10, 8):le_float()
		change_list:add(proto_sbe, buffer(offset, 18), change .. " " .. side .. " - price : " .. tostring(price) .. " amount : " .. tostring(amount))
		offset = offset + 18
	end

	return offset
end

function trades(buffer, ioffset, subtree)
	local offset = ioffset
	local message = subtree:add(proto_sbe, buffer(offset,12), "Trades")

	offset = message_header(buffer, offset, message)

	message:add_le(f_instrument_id, buffer(offset, 4))
	
	local group_block_len = buffer(offset + 4, 2):le_uint()
	local group_num_in_group = buffer(offset + 6, 2):le_uint()
	local group_size = group_block_len * group_num_in_group
	local change_group = message:add(proto_sbe, buffer(offset + 4, 8 + group_size), "Trades")
	local group_header = change_group:add(proto_sbe, buffer(offset + 4, 8 + group_size), "Group Header")

	group_header:add_le(f_group_block_len, buffer(offset + 4, 2))
	group_header:add_le(f_group_num_in_group, buffer(offset + 6, 2))
	group_header:add_le(f_group_num_groups, buffer(offset + 8, 2))
	group_header:add_le(f_group_num_vars, buffer(offset + 10, 2))

	offset = offset + 12

	local trade_list = change_group:add(proto_sbe, buffer(offset, group_size), "Trade list")

	for i=1, group_num_in_group do
		local trade = trade_list:add(proto_sbe, buffer(offset, 83), "Trade")
		trade:add(f_trade_direction, buffer(offset, 1))
		trade:add_le(f_price, buffer(offset + 1, 8))
		trade:add_le(f_amount, buffer(offset + 9, 8))

		offset = timestamp(buffer, offset + 17, trade, f_timestamp)

		trade:add_le(f_mark_price, buffer(offset, 8))
		trade:add_le(f_index_price, buffer(offset + 8, 8))
		trade:add_le(f_trade_seq, buffer(offset + 16, 8))
		trade:add_le(f_trade_id, buffer(offset + 24, 8))
		trade:add(f_tick_direction, buffer(offset + 32, 1))
		trade:add(f_liquidation, buffer(offset + 33, 1))
		trade:add_le(f_iv, buffer(offset + 34, 8))
		trade:add_le(f_block_trade_id, buffer(offset + 42, 8))
		trade:add_le(f_combo_trade_id, buffer(offset + 50, 8))

		offset = offset + 58
	end

	return offset

end

function ticker(buffer, ioffset, subtree)
	local offset = ioffset
	local message = subtree:add(proto_sbe, buffer(offset, 12), "Ticker")

	offset = message_header(buffer, offset, message)

	message:add_le(f_instrument_id, buffer(offset, 4))
	message:add_le(f_instrument_state, buffer(offset + 4, 1))

	offset = timestamp(buffer, offset + 5, message, f_timestamp)

	message:add_le(f_open_interest, buffer(offset, 8))
	message:add_le(f_min_sell_price, buffer(offset + 8, 8))
	message:add_le(f_max_buy_price, buffer(offset + 16, 8))
	message:add_le(f_last_price, buffer(offset + 24, 8))
	message:add_le(f_index_price, buffer(offset + 32, 8))
	message:add_le(f_mark_price, buffer(offset + 40, 8))
	message:add_le(f_best_bid_price, buffer(offset + 48, 8))
	message:add_le(f_best_bid_amount, buffer(offset + 56, 8))
	message:add_le(f_best_ask_price, buffer(offset + 64, 8))
	message:add_le(f_best_ask_amount, buffer(offset + 72, 8))
	message:add_le(f_current_funding, buffer(offset + 80, 8))
	message:add_le(f_funding_8h, buffer(offset + 88, 8))
	message:add_le(f_estimated_delivery_price, buffer(offset + 96, 8))
	message:add_le(f_delivery_price, buffer(offset + 104, 8))
	message:add_le(f_settlement_price, buffer(offset + 112, 8))

	return offset + 120
end

function snapshot(buffer, ioffset, subtree)
	local offset = ioffset
	local message = subtree:add(proto_sbe, buffer(ioffset, 12), "Book Snapshot")

	offset = message_header(buffer, offset, message)
	message:add_le(f_instrument_id, buffer(offset, 4))

	offset = timestamp(buffer, offset + 4, message, f_timestamp)

	message:add_le(f_change_id, buffer(offset, 8))
	message:add_le(f_is_book_complete, buffer(offset + 8, 1))
	message:add_le(f_is_last_part, buffer(offset + 9, 1))

	offset = offset + 10

	local group_block_len = buffer(offset, 2):le_uint()
	local group_num_in_group = buffer(offset + 2, 2):le_uint()
	local group_size = group_block_len * group_num_in_group
	local change_group = message:add(proto_sbe, buffer(offset, 8 + group_size), "Levels")
	local group_header = change_group:add(proto_sbe, buffer(offset, 8 + group_size), "Group Header")


	group_header:add_le(f_group_block_len, buffer(offset, 2))
	group_header:add_le(f_group_num_in_group, buffer(offset + 2, 2))
	group_header:add_le(f_group_num_groups, buffer(offset + 4, 2))
	group_header:add_le(f_group_num_vars, buffer(offset + 6, 2))

	offset = offset + 8

	local change_list = change_group:add(proto_sbe, buffer(offset, group_size), "Level List")
	local goffset = 0

	for i=1, group_num_in_group do
		local side = book_sides[buffer(offset, 1):uint()]
		local price = buffer(offset + 1, 8):le_float()
		local amount = buffer(offset + 9, 8):le_float()
		change_list:add(proto_sbe, buffer(offset, 17), side .. " - price : " .. tostring(price) .. " amount : " .. tostring(amount))
		offset = offset + 17
	end

	return offset

end

function message_header(buffer, ioffset, message)
	local offset = ioffset
	local messageHeader  = message:add(proto_sbe, buffer(offset, 12), "Header")
	messageHeader:add_le(f_block_length, buffer(offset,2))
	messageHeader:add_le(f_template_id, buffer(offset + 2,2))
	messageHeader:add_le(f_schema_id, buffer(offset + 4,2))
	messageHeader:add_le(f_version, buffer(offset + 6,2))
	messageHeader:add_le(f_num_groups, buffer(offset + 8,2))
	messageHeader:add_le(f_num_vars, buffer(offset + 10,2))
	return offset + 12
end

function timestamp(buffer, ioffset, message, field)
	local timestamp = buffer(ioffset, 8):le_uint64()
	local secs = math.floor(timestamp:tonumber() / 1000)
	local nsecs = math.fmod(timestamp:tonumber(), 1000) * 1000000
	message:add(field, buffer(ioffset, 8), NSTime.new(secs, nsecs))
	return ioffset + 8
end

function currency(buffer, ioffset, message, field)
	message:add(field, buffer(ioffset, 8))
	return ioffset + 8
end

--we register our protocol on UDP port 6100
DissectorTable.get("udp.port"):add(6100, proto_sbe)
DissectorTable.get("udp.port"):add(6101, proto_sbe)
