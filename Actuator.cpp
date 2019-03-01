#include "Actuator.h"

bool check_condition(actuator_t act)
{
	bool ret = false;
	Signal *a = nullptr;
	Signal *b = nullptr;
	int i;

	if (act.enabled)
	{
		for (i = 0; i < count && !ret; i++)
		{
			ret = true;
			for (int j = 0; j < 16; j++)
				ret &= signals[i]->label[j] == act.signal_a[j];
			if (ret)
				a = signals[i];
		}
		if (!ret)
		{
			for (i = 0; i < remotecount && !ret; i++)
			{
				ret = true;
				for (int j = 0; j < 16; j++)
					ret &= remotes[i]->label[j] == act.signal_a[j];
				if (ret)
					a = remotes[i];
			}
		}

		if (ret)
		{
			if (act.constcondition)
			{
				switch (act.comparator)
				{
				case EQUAL:
					ret = (a->Get() == act.constant);
					break;
				case NOT_EQUAL:
					ret = (a->Get() != act.constant);
					break;
				case LESS_THAN:
					ret = (a->Get() < act.constant);
					break;
				case LESS_THAN_EQUAL:
					ret = (a->Get() <= act.constant);
					break;
				case GREATER_THAN:
					ret = (a->Get() > act.constant);
					break;
				case GREATER_THAN_EQUAL:
					ret = (a->Get() >= act.constant);
					break;
				}
			}
			else
			{
				ret = false;
				for (i = 0; i < count && !ret; i++)
				{
					ret = true;
					for (int j = 0; j < 16; j++)
						ret &= signals[i]->label[j] == act.signal_b[j];
					if (ret)
						b = signals[i];
				}
				if (!ret)
				{
					for (i = 0; i < remotecount && !ret; i++)
					{
						ret = true;
						for (int j = 0; j < 16; j++)
							ret &= remotes[i]->label[j] == act.signal_b[j];
						if (ret)
							b = remotes[i];
					}
				}

				if (ret)
					switch (act.comparator)
					{
					case EQUAL:
						ret = (a->Get() == b->Get());
						break;
					case NOT_EQUAL:
						ret = (a->Get() != b->Get());
						break;
					case LESS_THAN:
						ret = (a->Get() < b->Get());
						break;
					case LESS_THAN_EQUAL:
						ret = (a->Get() <= b->Get());
						break;
					case GREATER_THAN:
						ret = (a->Get() > b->Get());
						break;
					case GREATER_THAN_EQUAL:
						ret = (a->Get() >= b->Get());
						break;
					}
			}
		}
	}

	return ret;
}