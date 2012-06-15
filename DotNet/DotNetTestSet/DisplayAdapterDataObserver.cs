﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using DNP3.Interface;
using TestSetControlLibrary;

namespace DotNetTestSet
{
    class DisplayAdapterDataObserver : IDataObserver
    {
        private MasterDataDisplay display;

        public DisplayAdapterDataObserver(MasterDataDisplay display)
        {
            this.display = display;
        }

        public void Start()
        {
            
        }

        public void Update(Binary update, uint index)
        {
            this.display.BeginInvoke(new Action(() => display.UpdateBinary(update, index)));
        }

        public void Update(Analog update, uint index)
        {
            this.display.BeginInvoke(new Action(() => display.UpdateAnalog(update, index)));
        }

        public void Update(Counter update, uint index)
        {
            this.display.BeginInvoke(new Action(() => display.UpdateCounter(update, index)));
        }

        public void Update(ControlStatus update, uint index)
        {
            this.display.BeginInvoke(new Action(() => display.UpdateBOStatus(update, index)));
        }

        public void Update(SetpointStatus update, uint index)
        {
            this.display.BeginInvoke(new Action(() => display.UpdateAOStatus(update, index)));
        }

        public void End()
        {

        }
    }
}
