modded class UApiEntityStore
{
	protected bool MapLink129ShouldLog()
	{
		return GetMapLinkConfig().EnableTransferDebugLogs;
	}

	protected void MapLink129Log(string message)
	{
		if (MapLink129ShouldLog())
		{
			MLLog.Info("[MapLink129] " + message);
		}
	}

	protected void MapLink129Warn(string message)
	{
		if (MapLink129ShouldLog())
		{
			MLLog.Err("[MapLink129] " + message);
		}
	}

	protected bool MapLink129ClassExists(string type)
	{
		if (!type || type == "")
		{
			return false;
		}

		if (GetGame().ConfigIsExisting(CFG_VEHICLESPATH + " " + type))
		{
			return true;
		}

		if (GetGame().ConfigIsExisting(CFG_WEAPONSPATH + " " + type))
		{
			return true;
		}

		if (GetGame().ConfigIsExisting(CFG_MAGAZINESPATH + " " + type))
		{
			return true;
		}

		return false;
	}

	protected bool MapLink129CanContinue(string reason)
	{
		MapLink129Warn("restore failure reason: " + reason);
		return GetMapLinkConfig().EnableSafeRestoreMode;
	}

	protected bool MapLink129ShouldRestoreWithParent(EntityAI parent)
	{
		if (!GetMapLinkConfig().EnableInventoryRestore)
		{
			MapLink129Log("item skipped type=" + m_Type + " reason=EnableInventoryRestore false");
			return false;
		}

		if (m_IsInHands && !GetMapLinkConfig().EnableHandsRestore)
		{
			MapLink129Log("item skipped type=" + m_Type + " reason=EnableHandsRestore false");
			return false;
		}

		if (parent && m_Slot == -1 && !GetMapLinkConfig().EnableCargoRestore)
		{
			MapLink129Log("item skipped type=" + m_Type + " reason=EnableCargoRestore false");
			return false;
		}

		if (parent && !PlayerBase.Cast(parent) && !GetMapLinkConfig().EnableContainerRestore)
		{
			MapLink129Log("item skipped type=" + m_Type + " reason=EnableContainerRestore false");
			return false;
		}

		if (parent && m_Slot != -1 && !m_IsInHands && !GetMapLinkConfig().EnableAttachmentRestore)
		{
			MapLink129Log("item skipped type=" + m_Type + " reason=EnableAttachmentRestore false");
			return false;
		}

		return true;
	}

	override EntityAI Create(EntityAI parent = NULL, bool RestoreOrginalLocation = true)
	{
		EntityAI item;

		if (!MapLink129ShouldRestoreWithParent(parent))
		{
			return NULL;
		}

		if (!MapLink129ClassExists(m_Type))
		{
			if (MapLink129CanContinue("class missing type=" + m_Type))
			{
				MapLink129Log("item skipped type=" + m_Type + " reason=class missing");
				return NULL;
			}
		}

		if (parent == NULL)
		{
			item = EntityAI.Cast(GetGame().CreateObject(m_Type, "0 0 0"));
		}
		else
		{
			if (!parent.GetInventory())
			{
				if (MapLink129CanContinue("parent inventory null parent=" + parent.GetType() + " child=" + m_Type))
				{
					MapLink129Log("item skipped type=" + m_Type + " reason=parent inventory null");
					return NULL;
				}
			}

			if (m_Slot == -1)
			{
				item = EntityAI.Cast(parent.GetInventory().CreateEntityInCargoEx(m_Type, m_Idx, m_Row, m_Col, m_Flip));
			}
			else if (m_IsInHands)
			{
				PlayerBase player = PlayerBase.Cast(parent.GetHierarchyRootPlayer());
				if (!player || !player.GetHumanInventory())
				{
					if (MapLink129CanContinue("hands parent invalid type=" + m_Type))
					{
						MapLink129Log("item skipped type=" + m_Type + " reason=hands parent invalid");
						return NULL;
					}
				}
				else
				{
					item = EntityAI.Cast(player.GetHumanInventory().CreateInHands(m_Type));
				}
			}
			else
			{
				item = EntityAI.Cast(parent.GetInventory().CreateAttachmentEx(m_Type, m_Slot));
			}
		}

		if (!item && parent)
		{
			item = EntityAI.Cast(GetGame().CreateObject(m_Type, parent.GetPosition()));
		}

		if (!item)
		{
			if (MapLink129CanContinue("spawn returned null type=" + m_Type))
			{
				MapLink129Log("item skipped type=" + m_Type + " reason=spawn returned null");
				return NULL;
			}
		}

		MapLink129Log("item restored type=" + m_Type);
		LoadEntity(item);
		return item;
	}

	override EntityAI CreateAtPos(vector Pos, vector Ori = "0 0 0")
	{
		if (!MapLink129ClassExists(m_Type))
		{
			if (MapLink129CanContinue("class missing type=" + m_Type))
			{
				MapLink129Log("item skipped type=" + m_Type + " reason=class missing");
				return NULL;
			}
		}

		EntityAI item = EntityAI.Cast(GetGame().CreateObject(m_Type, Pos));
		if (!item)
		{
			if (MapLink129CanContinue("CreateAtPos returned null type=" + m_Type))
			{
				MapLink129Log("item skipped type=" + m_Type + " reason=CreateAtPos returned null");
				return NULL;
			}
		}

		item.SetPosition(Pos);
		item.SetOrientation(Ori);
		MapLink129Log("item restored type=" + m_Type + " pos=" + Pos);
		LoadEntity(item);
		return item;
	}

	override void LoadEntity(EntityAI item)
	{
		if (!item)
		{
			MapLink129CanContinue("LoadEntity item null type=" + m_Type);
			return;
		}

		int i;
		item.SetHealth("", "", m_Health);
		ItemBase itemB;
		
		Weapon_Base weap;
		if (m_IsWeapon && Class.CastTo(weap, item))
		{
			int m_CurrentMuzzle = GetInt("m_CurrentMuzzle");
			if (m_CurrentMuzzle >= weap.GetMuzzleCount() || m_CurrentMuzzle < 0)
			{
				weap.SetCurrentMuzzle(m_CurrentMuzzle);
			}
		}

		if (m_Cargo && m_Cargo.Count() > 0)
		{
			if (!GetMapLinkConfig().EnableContainerRestore)
			{
				MapLink129Log("item skipped type=" + m_Type + " reason=EnableContainerRestore false for child restore");
			}
			else
			{
				for(i = 0; i < m_Cargo.Count(); i++)
				{
					if (!m_Cargo.Get(i))
					{
						MapLink129CanContinue("stored child null parent=" + m_Type);
						continue;
					}

					if (m_Cargo.Get(i).m_IsMagazine && m_IsWeapon && weap)
					{ 
						Magazine_Base child_mag = Magazine_Base.Cast(m_Cargo.Get(i).Create(item));
						if (weap && child_mag)
						{
							weap.AttachMagazine(weap.GetCurrentMuzzle(), child_mag);
						}
						else
						{
							MapLink129CanContinue("weapon magazine restore failed parent=" + m_Type);
						}
					}
					else 
					{
						m_Cargo.Get(i).Create(item);				
					}
				}
			}
		}

		if (Class.CastTo(itemB, item))
		{
			if (itemB.HasQuantity() && !itemB.IsMagazine())
			{
				itemB.SetQuantity(m_Quantity);
			}

			itemB.SetWet(m_Wet);
			itemB.SetTemperature(m_Tempature);
			itemB.SetLiquidType(m_LiquidType);
			if (itemB.GetCompEM())
			{
				itemB.GetCompEM().SetEnergy(m_Energy);
				if (m_IsOn)
				{
					itemB.GetCompEM().SwitchOn();
				}
			}

			itemB.RemoveAllAgents();
			itemB.TransferAgents(m_Agents);
			itemB.SetCleanness(m_Cleanness);
			itemB.OnUApiLoad(this);
		}

		PlayerBase HoldingPlayer;
		if (Class.CastTo(HoldingPlayer, item.GetHierarchyRootPlayer()))
		{
			if (m_QuickBarSlot >= 0)
			{
				HoldingPlayer.SetQuickBarEntityShortcut(item, m_QuickBarSlot);
			}
		}

		Magazine_Base mag;
		float dmg;
		string cartType;
		int count;
		if (m_IsMagazine && Class.CastTo(mag, item))
		{
			count = m_Quantity;
			mag.ServerSetAmmoCount(count);

			if (m_MagAmmo)
			{
				for (i = 0; i < mag.GetAmmoCount(); i++)
				{
					if (i >= m_MagAmmo.Count())
					{
						break;
					}

					if (m_MagAmmo.Get(i) && m_MagAmmo.Get(i).dmg() >= 0 && m_MagAmmo.Get(i).cartTypeName() != "" && m_MagAmmo.Get(i).cartIndex() == i)
					{
						mag.SetCartridgeAtIndex(m_MagAmmo.Get(i).cartIndex(), m_MagAmmo.Get(i).dmg(), m_MagAmmo.Get(i).cartTypeName());
					}
				}
			}
		}
		else if (item.IsAmmoPile() && Class.CastTo(mag, item))
		{
			count = m_Quantity;
			mag.ServerSetAmmoCount(count);
		}
		
		CarScript vehicle;
		if (m_IsVehicle && Class.CastTo(vehicle,item))
		{
			vehicle.OnUApiLoad(this);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(vehicle.Synchronize);
		}
		
		DamageZoneMap zones = new DamageZoneMap;
		DamageSystem.GetDamageZoneMap(item, zones);
		for(i = 0; i < zones.Count(); i++)
		{
			string zone = zones.GetKey(i);
			float health;
			if (m_HealthZones && ReadZoneHealth(zone, health))
			{
				item.SetHealth(zone, "", health);
			}
		}
		
		item.SetSynchDirty();
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(item.AfterStoreLoad);
	}
}
